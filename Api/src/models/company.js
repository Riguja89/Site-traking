const mongoose = require('mongoose');

const companySchema=mongoose.Schema({
    name:{
        type: String,
        default: "",
    },
    email:{
        type: String,
        required: true,
        unique: true,
    },
    nit:{
        type: String,
        required: true,
        unique: true,
    },
    compani_image: {
        type: String,
        default: "",
    },
    channels: [{
        type: mongoose.Types.ObjectId,
        ref: "Channels",
    }]  

});



module.exports = mongoose.model('Companies', companySchema)