const { Router } = require('express');
const usersRoute=require('./users');

// Importar todos los routers;
// Ejemplo: const authRouter = require('./auth.js');


const router = Router();

// Configurar los routers
// Ejemplo: router.use('/auth', authRouter);

// router.use('/videogames',videogamesRoute);
// router.use('/genres',genresRoute);
router.use('/users',usersRoute);

module.exports = router;
