require('dotenv').config();
const mongoose = require('mongoose');

//"mongodb+srv://<username>:<password>@cluster0.ixo4j.mongodb.net/?retryWrites=true&w=majority

mongoose.connect("mongodb+srv://user_sitetrack:o143gZKwWYXiuEvQ@cluster0.ixo4j.mongodb.net/?retryWrites=true&w=majority").then(()=>{
    console.log("conectado a la db")
})
.catch(error=> console.error(error))