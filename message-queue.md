# Message Queue

## Setup the message queue

    mqueue_init();

## Add a message to be sent.

    mqueue_add("GROUP", "OPERATION", "DATA");

## Register a callback function for incoming messages of a particular group.

    mqueue_register_handler("GROUP", msg_handler);