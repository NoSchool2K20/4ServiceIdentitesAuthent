/*
 ** knex is an excellent query manager in JS http://knexjs.org/
 ** bs-knex is unmaintaned and incomplete, but existing binding are good to manage connection
 ** We can use rawBinding query which is a safe raw query instead of any unexisting bindings
 ** While SQLite is not really suitable for production this part is more a POC to facilitate the conception of an API.
 */

let connection =
  KnexConfig.Connection.make(~filename="./database/yatdb.sqlite", ());

let config =
  KnexConfig.make(
    ~client="sqlite3",
    ~connection,
    ~acquireConnectionTimeout=2000,
    (),
  );

let knex = Knex.make(config);

module Todos = {
  let getAll = () =>
    Js.Promise.(
      knex
      |> Knex.fromTable("todos")
      |> Knex.toPromise
      |> then_(results => {
           Model.Todos.fromJson(results)
           |> List.map(todo => {
                Model.Todo.make(
                  Model.Todo.getId(todo),
                  Model.Todo.getDescription(todo),
                  Model.Todo.isCompleted(todo),
                )
              })
           |> Model.Todos.toJson
           |> resolve
         })
    );

  let getByCompletness = cfilter =>
    Js.Promise.(
      knex
      |> Knex.fromTable("todos")
      |> Knex.toPromise
      |> then_(results => {
           Model.Todos.fromJson(results)
           |> List.map(todo => {
                Model.Todo.make(
                  Model.Todo.getId(todo),
                  Model.Todo.getDescription(todo),
                  Model.Todo.isCompleted(todo),
                )
              })
           |> Model.Todos.filterByCompletness(cfilter)  // to vary composition exemple, WHERE clause is better
           |> Model.Todos.toJson
           |> resolve
         })
    );

  let getById: string => Js.Promise.t(Js.Json.t) =
    id =>
      Js.Promise.(
        knex
        |> Knex.fromTable("todos")
        |> Knex.where({"id": id})
        |> Knex.toPromise
        |> then_(results => {
             Model.Todos.fromJson(results)
             |> List.map(todo => {
                  Model.Todo.make(
                    Model.Todo.getId(todo),
                    Model.Todo.getDescription(todo),
                    Model.Todo.isCompleted(todo),
                  )
                })
             |> Model.Todos.toJson
             |> resolve
           })
      );

  let update = (id, description, completed) => {
    let todo = Model.Todo.make(id, description, completed);
    Js.Promise.(
      knex
      |> Knex.rawBinding(
           "UPDATE todos SET DESCRIPTION=? , COMPLETED=? WHERE ID=?",
           (
             Model.Todo.getDescription(todo),
             Model.Todo.isCompleted(todo),
             Model.Todo.getId(todo),
           ),
         )
      |> Knex.toPromise
      |> then_(_ => {resolve()})
    );
  };

  let create = description => {
    let todo = Model.Todo.makeNew(description);
    Js.Promise.(
      knex
      |> Knex.rawBinding(
           "INSERT INTO todos VALUES ( ? , ? , ?)",
           (
             Model.Todo.getId(todo),
             Model.Todo.getDescription(todo),
             Model.Todo.isCompleted(todo),
           ),
         )
      |> Knex.toPromise
      |> then_(_ => {resolve()})
    );
  };
};