require('dotenv').config();
const mongoose = require('mongoose');

mongoose.connect("mongodb+srv://<username>:<password>@cluster0.ixo4j.mongodb.net/?retryWrites=true&w=majority").then(()=>{
    console.log("conectado a la db")
})
.catch(error=> console.error(error))