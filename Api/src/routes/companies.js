const { Router } = require('express');
const router = Router();
const companySchema = require("../models/company")

router.get('/',async(req, res, next)=>{
    companySchema.find()
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
   
})

router.put('/:id',(req, res, next)=>{
    const {id}=req.params;
    companySchema.updateOne({_id:id},{ $set:req.body})
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
})

// Create Company
 router.post('/',async (req, res, next)=>{
    const company=companySchema(req.body);
    company.save()
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
 })

router.delete('/:id',(req, res, next)=>{
    const {id}=req.params;
    companySchema.remove({_id:id})
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
})

// Buscar company por Id
router.get('/:id',async(req, res, next)=>{
    const {id}=req.params;
    companySchema.findById(id)
    .then(data=>res.json(data))
    .catch(error=>res.json({message: error}))
   
})



module.exports = router;