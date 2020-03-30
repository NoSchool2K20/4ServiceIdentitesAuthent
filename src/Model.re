[@bs.module "uuid"] external uuidv4: unit => string = "v4";

module Todo: {
  type t; // this is an abastraction to enforce creation with make functions
  let make: (string, string, bool) => t;
  let makeNew: string => t;
  let modifyDescription: (string, t) => t;
  let complete: t => t;
  let uncomplete: t => t;
  let getId: t => string;
  let getDescription: t => string;
  let isCompleted: t => bool;
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = {
    id: string,
    description: string,
    completed: bool,
  };

  // create a value of the type Todo.t
  let make = (id, description, completed) => {id, description, completed};
  let makeNew = description => make(uuidv4(), description, false);

  // Getters are mandatory because of abstraction,
  // while it is a good practice for accessing records members, like for object members
  let getId = todo => todo.id;
  let getDescription = todo => todo.description;
  let isCompleted = todo => todo.completed;

  // Setters
  let modifyDescription = (newDesc, todo) => {...todo, description: newDesc};
  let complete = todo => {
    {...todo, completed: true};
  };
  let uncomplete = todo => {
    {...todo, completed: false};
  };

  /* utility fonction for boolean repesented as 0 / 1 */
  let bool_of_int = value => value === 1;
  let fromJson = json =>
    Json.Decode.{
      id: json |> field("ID", string),
      description: json |> field("DESCRIPTION", string),
      completed: json |> field("COMPLETED", int) |> bool_of_int,
    };

  let fromString = jsonString =>
    switch (Json.parse(jsonString)) {
    | Some(validJson) => Some(fromJson(validJson))
    | None => None
    };

  let toJson = todo =>
    // Json module comes from bs-json, it is not Js.Json module
    // Use of bs-json to encode json in expressive way while you can use Js.Json / Js.Dict in an imperative way
    Json.Encode.(
      object_([
        ("id", string(todo.id)),
        ("description", string(todo.description)),
        ("completed", bool(todo.completed)),
      ])
    );
  let toString = todo => toJson(todo) |> Js.Json.stringify;
};

module Todos: {
  type t = list(Todo.t); // do not overuse abstraction when unecessary
  let filterByDescription: (string, t) => t;
  let filterByCompletness: (bool, t) => t;
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = list(Todo.t);

  let filterBy:
    (~description: option(string), ~completed: option(bool), t) => t =
    (~description, ~completed, todos) => {
      let descFiltered =
        switch (description) {
        | None => todos
        | Some(desc) =>
          List.filter(
            item =>
              // Use Js.String her because Buckelscript do not include Pervasive.Str from reason
              // Simplify string manipulation without Str module avalaible
              Js.String.(
                includes(make(desc), make(Todo.getDescription(item)))
              ),
            todos,
          )
        };

      switch (completed) {
      | None => descFiltered
      | Some(b) =>
        List.filter(item => Todo.isCompleted(item) === b, descFiltered)
      };
    };

  let filterByDescription = description =>
    filterBy(~description=Some(description), ~completed=None);
  let filterByCompletness = completed =>
    filterBy(~description=None, ~completed=Some(completed));

  let fromJson: Js.Json.t => t =
    json => json |> Json.Decode.(list(Todo.fromJson));

  let fromString = jsonString =>
    switch (Json.parse(jsonString)) {
    | Some(validJson) => Some(fromJson(validJson))
    | None => None
    };

  let toJson = todos =>
    Array.of_list(todos)  // convert list(Todo.t) to array(Todo.t)
    |> Array.map(item => Todo.toJson(item))  // create an array(Js.Json.t)
    |> Json.Encode.jsonArray;
  let toString = todos => toJson(todos) |> Js.Json.stringify;
};