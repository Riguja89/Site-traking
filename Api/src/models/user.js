const mongoose = require('mongoose');
var bcrypt = require('bcryptjs');

const userSchema=mongoose.Schema({
    name:{
        type: String,
        default: "",
    },
    lastname: {
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
    user_image: {
        type: String,
        default: "",
    },
    banned: {
        type: Boolean,
        default: false,
      },
    company: {
        type: mongoose.Types.ObjectId,
        ref: "Companies",
    },
    role: {
        type: mongoose.Types.ObjectId,
        ref: "Rols",
    }   

});

userSchema.statics.encryptPassword = async (password)=> {
    const salt = await bcrypt.genSalt(10);
    return bcrypt.hash(password, salt);
  };

userSchema.statics.comparePassword = async (password, receivedPassword)=> {
    return await bcrypt.compare(password, receivedPassword);
  };  


module.exports = mongoose.model('User', userSchema)