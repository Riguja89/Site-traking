const { Router } = require('express');
const axios=require('axios');
const {Genre}=require('../db.js');
const {API_KEY} = process.env;
const router = Router();

router.get('/',async(req, res, next)=>{
    // var genres=[]
    // try {
    //     genres=await Genre.findAll();
    //     genres.sort(function(a,b){
    //         if (a.name.toLowerCase() > b.name.toLowerCase()) {
    //             return 1;
    //           }
    //           if (a.name.toLowerCase() < b.name.toLowerCase()) {
    //             return -1;
    //           }
    //           return 0; 
    //     })
    //     res.json(genres)
    // } catch (error) {
    //     next(error)
    // }

      res.send('Soy get/users')
})

router.put('/',(req, res, next)=>{
    res.send('Soy put/genre')
})

// router.post('/',async (req, res, next)=>{
//     try {
//         let genres=await axios.get('https://api.rawg.io/api/genres?key='+API_KEY+'&page_size=100');
//         genres=genres.data.results.map(result=>{
//             return{
//                 id: result.id,
//                 name: result.name,
//                 image:result.image_background
//             }
//         })
        
//         res.json(await Genre.bulkCreate(genres));
//     } catch (error) {
//         next(error)
//     }

// })

router.delete('/',(req, res, next)=>{
    res.send('Soy delete/genre')
})


module.exports = router;