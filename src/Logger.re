/**
 * Never use Js.log in production http://expressjs.com/en/advanced/best-practice-performance.html#do-logging-correctly
 * We will use winston https://github.com/winstonjs/winston
 */
// Some module alias to be less verbose
module L = BsWinston.Logger;
module T = BsWinston.Transport;
module F = BsWinston.Format;
module B = BsWinston.Builder;

let levelString =
  "LEVEL"
  |> Js.Dict.get(Node.Process.process##env)
  |> Js.Option.getWithDefault("info");
let level =
  if (levelString === "debug") {
    B.Debug;
  } else if (levelString === "warn") {
    B.Warn;
  } else if (levelString === "verbose") {
    B.Verbose;
  } else if (levelString === "error") {
    B.Error;
  } else {
    Info;
  };

let logger =
  B.(
    create()
    ->setLevel(level) // -> is pipe first operator (only work with Buckelscript not with native) https://reasonml.github.io/docs/en/pipe-first
    ->addFormat(
        F.combine([
          F.createTimestamp(),
          F.createPrettyPrint(~colorize=true, ()),
        ]),
      )
    ->addTransport(T.createConsole())
    ->build
  );

let log: (L.t => L.t, string) => unit =
  (loggerLevel, message) =>
    logger->loggerLevel->L.withMessage(message)->L.log;

let debug = log(L.debug);
let info = log(L.info);
let warn = log(L.warn);
let error = log(L.error);