const { Router } = require('express');
const usersRoute=require('./users');
const companyRoute=require('./companies')

// Importar todos los routers;
// Ejemplo: const authRouter = require('./auth.js');


const router = Router();

// Configurar los routers
// Ejemplo: router.use('/auth', authRouter);

// router.use('/videogames',videogamesRoute);
router.use('/companies',companyRoute);
router.use('/users',usersRoute);

module.exports = router;
