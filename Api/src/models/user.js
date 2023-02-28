const mongoose = require('mongoose');

const userSchema=mongoose.Schema({
    name:{
        type: String,
        default: "",
    },
    password:{
        type: String,
        required: true,
    },
    email:{
        type: String,
        required: true,
        unique: true,
    },
    lastname: {
        type: String,
        default: "",
      },
    user_image: {
        type: String,
        default: "",
    },
    banned: {
        type: Boolean,
        default: false,
      },
    company: [{
        type: mongoose.Types.ObjectId,
        ref: "Companies",
    }]  

});

module.exports = mongoose.model('User', userSchema)