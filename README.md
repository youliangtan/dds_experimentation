# dds_experimentation

We will check if various forms of msg with different typename and fields can be communicated across pub sub nodes. Msgs are shown below:

### Msg Definition

1) Message: `HelloWorld_v1.idl`

<table>
<tr>
<td> Type 1 </td> <td> Type 2 </td> <td> Type 3 </td>
</tr>
<tr>
<td>

```cpp
struct HelloWorld_v1
{
  unsigned long index;
  string message;
};
```

</td>
<td>

```cpp
struct HelloWorld_v1
{
  unsigned long index;
  string message;
  float value;
};
```
</td>

<td>

```cpp
struct HelloWorld_v1
{
  short index;
  string message;
};
```
</td>
</tr>
</table>


2) Message `HelloWorld_v2.idl`

<table>
<tr>
<td> Type 1 </td> <td> Type 2 </td> <td> Type 3 </td>
</tr>
<tr>
<td>

```cpp
struct HelloWorld_v2
{
  unsigned long index;
  string message;
};
```

</td>
<td>

```cpp
struct HelloWorld_v2
{
  unsigned long index;
  string message;
  float value;
};
```
</td>

<td>

```cpp
@appendable
struct HelloWorld_v2
{
  unsigned long index;
  string message;
  float value;
};
```
</td>
</tr>
</table>

3) Message `HelloWorld_v3.idl`

<table>
<tr>
<td> Type 1 </td> <td> Type 2 </td> <td> Type 3 </td>
</tr>
<tr>
<td>

```cpp
@appendable
struct HelloWorld_v3
{
  unsigned long index;
  string message;
  float value;
  sequence<double> data;
};
```

</td>
<td>

```cpp
@appendable
struct HelloWorld_v3
{
  unsigned long index;
  string message;
  short value;
};
```
</td>

<td>

```cpp
@appendable
struct HelloWorld_v3
{
  unsigned long index;
  float message;
  short value;
};
```
</td>

</tr>
</table>


### Comparison Table

|    |      Fast DDS      |  Cyclone DDS |  RTI Connext |
|----------|:--:|:--:|:--:|
| **Same Type Name**: <br> `HelloWorld_v1.idl` with `HelloWorld_v1.idl` |
| `V1T1` with `V1T2` | &check; | &cross; | &check; |
| `V1T1` with `V1T3` | &check; | &cross; | &cross; |
| `V1T2` with `V1T3` | &check; | &cross; | &cross; |
| **Mismatch Type Name** <br> `HelloWorld_v1.idl` with `HelloWorld_v2.idl` |
| `V1T1` with `V2T1` | &cross; | &cross; | &check; |
| `V1T1` with `V2T2` | &cross; | &cross; | &check; |
| `V1T1` with `V2T3` | &cross; | &cross; | &check; |
| **Extensibility with mismatch type name** `HelloWorld_v2.idl` with `HelloWorld_v3.idl` |||
| `V2T3` with `V3T1` | &cross; | &check; | &check; |
| `V2T3` with `V3T2` | &cross; | &cross; | &check; |
| `V2T3` with `V3T3` | &cross; | NA | &cross; |

---

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
```bash
cd dds_experimentation/fastdds/HelloWorldExample
java -jar ~/fastdds_ws/src/fastddsgen/share/fastddsgen/java/fastddsgen.jar HelloWorld_v1.idl -replace
```

### Testing with incompatible topic detection

Modified the HelloWorldExample script (`~/Fast-DDS/examples/C++/DDS/HelloWorldExample/build`), and let the publisher publishes an incompatible topic. 

Provide listener class member to `create_topic()` function. However, currently the `TopicListener::on_inconsistent_topic()` callback is not working as it isn't yet being implemented (refer [here](https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/topic/topicListener/topicListener.html)). 


### Parsing UserDataQosPolicy

Working in progress in `HellowWorldExample`

---

## RTI Connext

### Setting up env
Install RTI:

Download and install, 6.X.X: https://www.rti.com/free-trial/dds-files.
Select the provided rti license during installation via the `RTI Launcher` gui

## on terminal
```bash
export NDDSHOME=~/rti_connext_dds-6.1.1
export PATH=${PATH}:~/rti_connext_dds-6.1.1/lib/x64Linux4gcc7.3.0
```

Then compile in `build` dir
```bash
cmake ..
make -j4
```

### Testing with incompatible topic detection

A modified example code of [listeners](https://github.com/rticommunity/rticonnextdds-examples/tree/master/examples/connext_dds/listeners/c%2B%2B11)

After compile with cmake (`cd builld && cmake .. && cmake --build .`), then run:
```bash
./listeners_subscriber
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

### Hello world example:

Generate headers from `idl`
```bash
cd hello_world_example
mkdir build cd build
cmake ..
make -j4
```

Run
```bash
example_publisher
example_subscriber
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

Run
```
./HelloworldPublisher
./HelloworldSubscriber
```

Unfortunately, not able to trigger `on_inconsistent_topic` callback

The above example also shows the usage of dds's extensibility by using `@appendable` in 2 different .idl definitions for pub sub.
