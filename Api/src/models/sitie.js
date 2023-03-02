const mongoose = require('mongoose');

const sitieSchema=mongoose.Schema({
    name:{
        type: String,
        default: "",
    },
    channel:{
        type: mongoose.Types.ObjectId,
        ref: "Channels",
    },
    field:{
        type: Number,
        required:true,
    }  

});



module.exports = mongoose.model('Sities', sitieSchema)