// Help secure Express apps with various HTTP headers : https://helmetjs.github.io/
// Default config is just fine, don't need complex bindings
[@bs.module] external helmet: unit => Express.Middleware.t = "helmet";
// Compress response body https://github.com/expressjs/compression#readme
[@bs.module]
external compression: unit => Express.Middleware.t = "compression";

open Express;

// bs-express lacks http://expressjs.com/en/4x/api.html#express.json
[@bs.module "express"] external json: unit => Middleware.t = "json";

// INSTANCIATE AN EXPRESS APPLICATION
let app = express();

// SETUP MIDDLEWARES
App.use(app, helmet());
App.use(app, compression());
App.use(app, json());
// setup a middelware that log all access at info level - not GDPR compliant :)
App.use(app, Controller.logRequest);

// SETUP ROUTES
App.get(app, ~path="/", Controller.welcome);

App.get(app, ~path="/todos", Controller.Todos.getAll);
App.get(app, ~path="/todos/:id", Controller.Todos.get);

App.post(app, ~path="/todos", Controller.Todos.create);
App.put(app, ~path="/todos/:id", Controller.Todos.update);

App.useOnPath(app, ~path="*", Controller.badRessource);