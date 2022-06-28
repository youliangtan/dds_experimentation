# dds_experimentation
testing with dds vendors

## Fast DDS
### Create Env

Refered to [eproxima official doc](https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html)

```bash
mkdir fastdds_ws
cd fastdds_ws
wget https://raw.githubusercontent.com/eProsima/Fast-DDS/master/fastrtps.repos
mkdir src
vcs import src < fastrtps.repos
```

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

The msg definition is defined in the `example_type.xml`. The example shows the publisher publishes `CustomMsgFoo` and `CustomMsgBar` msg types. And the sub will only able to sub to one type during discovery. Note that the msg definition is parsed via dds from the pub to sub participant.

### Static Types
### Generate headers from IDL

Uses `fastddsgen`

Quick installation, follow: https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#fast-dds-gen-installation

After installations and compilations
```
cd fastddsgen/share/fastddsgen/java
java -jar fastddsgen.jar HelloWorld.idl
```

### Testing with incompatible topic detection

Modified the HelloWorldExample script (`~/Fast-DDS/examples/C++/DDS/HelloWorldExample/build`), and let the publisher publishes an incompatible topic. 

Provide listener class member to `create_topic()` function. However, currently the `TopicListener::on_inconsistent_topic()` callback is not working as it isn't yet being implemented (refer [here](https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/topic/topicListener/topicListener.html)). 


### Parsing UserDataQosPolicy

Working in progress in `HellowWorldExample`

## RTI Connext

### Setting up env
Install RTI:

Download and install, 6.X.X: https://www.rti.com/free-trial/dds-files\. 
Select the provided rti license during installation via the gui

## on terminal
```
export NDDSHOME=~/rti_connext_dds-6.1.1
export PATH=${PATH}:~/rti_connext_dds-6.1.1/lib/x64Linux4gcc7.3.0
```

### Testing with incompatible topic detection

A modified example code of [listeners](https://github.com/rticommunity/rticonnextdds-examples/tree/master/examples/connext_dds/listeners/c%2B%2B11)

After compile with cmake, then run:
```
./listeners_subsriber
./listeners_publisher
```

publisher temp creates an incompatible topic, which will trigger subscriber to call `on_inconsistent_topic`.

Printout from subscriber, notice the `on_inconsistent_topic` callback
```
youliang@youliang-XPS-13-7390:~/rticonnextdds-examples/examples/connext_dds/listeners/c++11/build$ ./listeners_subscriber
RTI Connext DDS EVAL License issued to open robotics youliang@openrobotics.org For non-production use only.
Expires on 01-Jul-2022 See www.rti.com for more information.
ParticipantListener: on_inconsistent_topic()!!!
 msg def::topic_listeners listeners

ReaderListener: on_subscription_matched()
ReaderListener: on_liveliness_changed()
  Alive writers: 1
SubscriberListener: on_data_on_readers()
[x: 1]
SubscriberListener: on_data_on_readers()
[x: 2]
```

---

## Cyclonedds

Install with colcon, refer to [this](https://docs.ros.org/en/foxy/Installation/DDS-Implementations/Working-with-Eclipse-CycloneDDS.html#build-from-source-code)

Testout modified [helloworld](https://github.com/eclipse-cyclonedds/cyclonedds/tree/master/examples/helloworld) code, with `on_inconsistent_topic` callback of cyclonedds C api, defined [here](https://github.com/eclipse-cyclonedds/cyclonedds/blob/363e47598262011e1ad386f16cbb4a28c48ff13d/src/core/ddsc/include/dds/ddsc/dds_public_listener.h#L32)

```
source ~/cyclonedds_ws/install/setup.bash
mkdir build
cmake ..
make -j4
```


Unfortunately, not able to trigger `on_inconsistent_topic` callback

