const { Router } = require('express');
const axios=require('axios');
const {Genre}=require('../db.js');
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

 router.post('/',async (req, res, next)=>{

    const user=userSchema(req.body);
    user.save()
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
   
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