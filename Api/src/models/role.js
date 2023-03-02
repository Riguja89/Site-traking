const mongoose = require('mongoose');

const roleSchema=mongoose.Schema({
    name:{
        type: String,
        required:true,
        unique: true
    }
});



module.exports = mongoose.model('Rols', roleSchema)