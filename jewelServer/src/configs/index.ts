import dotenv from 'dotenv';

// '.env' File Loading
dotenv.config();

const NODE_ENV: string = process.env.NODE_ENV || 'dev';

if (NODE_ENV === 'prod') dotenv.config({ path: `${__dirname}/../../.env.prod` });
else if (NODE_ENV === 'dev') dotenv.config({ path: `${__dirname}/../../.env.dev` });
else if (NODE_ENV === 'test') dotenv.config({ path: `${__dirname}/../../.env.test` });

// ENV Module
export default {
  NODE_ENV,
  PORT: Number(process.env.PORT),
};