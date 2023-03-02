const mongoose = require('mongoose');

const channelSchema=mongoose.Schema({
    _id:{
        type: Number,
        required:true,
        unique: true
    },
    read_ApiKey:{
        type: String,
        required:true,
        unique: true
    } ,
    write_ApiKey:{
        type: String,
        required:true,
        unique: true
    } ,
    sities:[{
        type: mongoose.Types.ObjectId,
        ref: "Sities",
    }],
    company:{
        type: mongoose.Types.ObjectId,
        ref: "Companies",
    },
});



module.exports = mongoose.model('Channels', channelSchema)