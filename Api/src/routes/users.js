const { Router } = require('express');
const axios=require('axios');
const jwt=require('jsonwebtoken');
const {API_KEY} = process.env;
const router = Router();
const userSchema = require("../models/user")

router.get('/',async(req, res, next)=>{
    userSchema.find()
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
   
})

router.put('/:id',(req, res, next)=>{
    const {id}=req.params;
    userSchema.updateOne({_id:id},{ $set:req.body})
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
})

// Create User
 router.post('/',async (req, res, next)=>{
    const user=userSchema(req.body);
    user.password= await userSchema.encryptPassword(req.body.password);
    user.save()
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
 })

 // Login User
 router.post('/login',async (req, res, next)=>{
    const {email, password} = req.body;
    try {
        const userFound= await userSchema.findOne({
            email:email,
          });
          if (!userFound) return res.status(400).json({ message: "Invalid user or password" });
          const matchPassword = await userSchema.comparePassword(password,userFound["password"]);

          if (userFound["banned"] === true) {
            return res.status(400).json({ message: "User banned" });
          };

          if (!matchPassword)
          return res.status(401).json({ token: null, message: "Invalid user or password" });

          const token = jwt.sign({ _id: userFound["_id"] }, "token", {
            expiresIn: 60 * 60 * 12,
          });

          const response = {
            user: userFound,
            token,
          };

          res.header("auth-token", token).send(response);

    } catch (err) {
        res.status(400).json(err)
    }
 })



router.delete('/:id',(req, res, next)=>{
    const {id}=req.params;
    userSchema.remove({_id:id})
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
})

// Buscar usuario por Id
router.get('/:id',async(req, res, next)=>{
    const {id}=req.params;
    userSchema.findById(id)
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
   
})



module.exports = router;