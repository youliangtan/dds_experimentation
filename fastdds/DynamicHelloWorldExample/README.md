### Testing dynamic msg pubsub

Refered to [this](https://fast-dds.docs.eprosima.com/en/latest/fastdds/dynamic_types/examples.html) from eproxima

```bash
cd /fastdds_ws
source install/setup.bash 
cd src/dds_experimentation/fastdds/DynamicHelloWorldExample
mkdir build
cmake ..
make -j4
```

Run pub sub in 2 different terminals
```bash
# In the first one launch: 
./DDSDynamicHelloWorldExample publisher
# In the second one: 
./DDSDynamicHelloWorldExample subscriber
```

The msg definition is defined in the `example_type.xml`. The example shows the publisher publishes `CustomMsgFoo` and `CustomMsgBar` msg types. And the sub is set to only sub to one type during discovery. N

In this example, the publisher loads a type from the XML file "example_type.xml". The publisher shares the TypeObject so another participants can discover it.

The subscriber will discover the type and introspecting it will show the received messages from the publisher.
For simplicity the subscriber doesn't parses wstring, union, sequences or arrays.

The type defined in the XML file can be modified to see how the subscriber notices the changes automatically,
but take into account that the publisher should be modified to understand the changes.
