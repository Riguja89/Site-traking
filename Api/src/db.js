require('dotenv').config();
const mongoose = require('mongoose');
const {DB_USER,DB_PASSWORD}=process.env;

const options={
    useNewUrlParser:true,
    useUnifiedTopology:true,
    useCreateIndex:true,
}
mongoose.set('strictQuery', true);

mongoose.connect(`mongodb+srv://${DB_USER}:${DB_PASSWORD}@cluster0.ixo4j.mongodb.net/?retryWrites=true&w=majority`).then(()=>{
    console.log("conectado a la db")
})
.catch(error=> console.error(error))