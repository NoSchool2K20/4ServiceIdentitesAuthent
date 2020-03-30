/**
 * Start listening on port 3000
 */
let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    (
      switch (Js.Exn.message(e)) {
      | None => "UNKNOWN ERROR"
      | Some(msg) => msg
      }
    )
    |> Logger.error;
    Node.Process.exit(1);
  | _ => "Listening at http://127.0.0.1:8080" |> Logger.info
  };

let server = Express.App.listen(Api.app, ~port=8080, ~onListen, ());