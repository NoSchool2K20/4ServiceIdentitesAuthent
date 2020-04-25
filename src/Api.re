// Help secure Express apps with various HTTP headers : https://helmetjs.github.io/
// Default config is just fine, don't need complex bindings
[@bs.module] external helmet: unit => Express.Middleware.t = "helmet";
// Compress response body https://github.com/expressjs/compression#readme
[@bs.module]
external compression: unit => Express.Middleware.t = "compression";

open Express;

// bs-express lacks http://expressjs.com/en/4x/api.html#express.json
[@bs.module "express"] external json: unit => Middleware.t = "json";
[@bs.module] external cors: unit => Express.Middleware.t = "cors";


// INSTANCIATE AN EXPRESS APPLICATION
let app = express();

// SETUP MIDDLEWARES
App.use(app, helmet());
App.use(app, compression());
App.use(app, json());
App.use(app, cors());
// setup a middelware that log all access at info level - not GDPR compliant :)
App.use(app, Controller.logRequest);

// SETUP ROUTES
// Example with welcome route
App.getWithMany(app, ~path="/", [|Controller.authenticate, "Nouveau" |> Controller.permit, Controller.welcome|]);

App.get(app, ~path="/users", Controller.Users.getAll);
App.get(app, ~path="/users/:email", Controller.Users.get);

App.post(app, ~path="/verify", Controller.Users.verify);

App.post(app, ~path="/users", Controller.Users.create);

App.post(app, ~path="/connection", Controller.Users.connection);

App.put(app, ~path="/users/:email", Controller.Users.update);


App.get(app, ~path="/assignmentRequest", Controller.AssignmentRequest.getAll);
App.get(app, ~path="/assignmentRequest/processed", Controller.AssignmentRequest.getAllWithProcessed(true));
App.get(app, ~path="/assignmentRequest/notprocessed", Controller.AssignmentRequest.getAllWithProcessed(false));
App.postWithMany(app, ~path="/assignmentRequest", [|Controller.authenticate, "Nouveau" |> Controller.permit,Controller.AssignmentRequest.create|]);
App.postWithMany(app, ~path="/assignmentRequest/:uuid/accept", [|Controller.authenticate, "Administrateur" |> Controller.permit,true |> Controller.AssignmentRequest.acceptOrDecline|]);
App.postWithMany(app, ~path="/assignmentRequest/:uuid/decline", [|Controller.authenticate, "Administrateur" |> Controller.permit,false |> Controller.AssignmentRequest.acceptOrDecline|]);


App.useOnPath(app, ~path="*", Controller.badRessource);