# The Model

Think of it this way: The Editor interacts only with the Qt Model, which in turn interacts with the protobuf using Reflection.

Reflection is a powerful tool that allows us to interact with a protobuf generated class without knowing its structure. This is useful because protobuf is a dynamic structure that can change at any time. For example, assume we have an unknown class with unknown fields. Reflection allows us to interact with this class and its fields without knowing anything about them. Setting, getting, and adding fields is a breeze with Reflection.

When I personally work with the protobuf, I keep 4 tabs open:
1. https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
2. https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
3. https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
4. https://doc.qt.io/qt-5/qabstractitemmodel.html

### Make sure you give this a read: https://protobuf.dev/programming-guides/best-practices/

You might be wondering why I used Protobuf? It seems like the Qt Model is doing all the work. Well, that's because of two reasons:
1. This project is a simple version of a much larger project called [RadialGM](https://github.com/enigma-dev/RadialGM). RadialGM uses the protobuf many ways. For example, it uses the protobuf to be compatible with older versions of the engine. Oh! yeah, RadialGM is an IDE for the ENIGMA game engine. Also, ENIGMA is used as a separate local (can be distributed as well) server and RGM communicates with it using gRPC and Protobuf. My mentor, Robert B. Colton, told me that you can't count how many ways you can use protobuf. It's a powerful tool that can be used in many ways.

2. I am tired of writing modules by myself so I decided to use Protobuf's built-in serialization and deserialization. This way I can save/load the model easily.
3. When I figure out the 3rd reason, I will let you know, heh.
