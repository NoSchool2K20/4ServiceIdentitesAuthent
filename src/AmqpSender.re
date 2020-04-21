module Amqp = AmqpConnectionManager;

let queue_name = "Qnewuser";
let amqp_u = "qzscetiz"
let amqp_p = "iLJmX80CVSklfcVeS1NH81AwaHLSikPh"
let amqp_host = "crow.rmq.cloudamqp.com"
let amqp_url = "amqp://"++amqp_u++":"++amqp_p++"@"++amqp_host++"/"++amqp_u

// Create a connetion manager
let connection = Amqp.connect([|amqp_url|], ());

Amqp.AmqpConnectionManager.on(
  connection,
  `disconnect(err => Js.Console.error(err)),
)
|> ignore;

Amqp.AmqpConnectionManager.on(
  connection,
  `connect(_ => Js.Console.info("connected!")),
)
|> ignore;

// Set up a channel listening for messages in the queue.
let channelWrapper = queue_name =>
  Amqp.AmqpConnectionManager.createChannel(
    connection,
    {
      "json": true,
      "setup": channel =>
        // `channel` here is a regular amqplib `ConfirmChannel`.
        Js.Promise.(
          all([|
            Amqp.Channel.assertQueue(channel, queue_name, {"durable": true})
            |> then_(_ => resolve()),
          |])
          |> then_(_ => resolve())
        ),
    },
  );

let sendMessage = (queue_name, message) => {
  Amqp.ChannelWrapper.sendToQueue(
    channelWrapper(queue_name),
    queue_name,
    message,
    Js.Obj.empty(),
  )
  |> Js.Promise.then_(msg => {
       Js.Console.info("Message sent");
       msg |> Js.Promise.resolve;
     })
  |> Js.Promise.then_(_ => Js.Promise.resolve())
  |> Js.Promise.catch(err => {
       Js.Console.error(err);
       Amqp.ChannelWrapper.close(channelWrapper(queue_name));
       Amqp.AmqpConnectionManager.close(connection);
       Js.Promise.resolve();
     });
};

let sendNewUser = user => sendMessage(queue_name, user);