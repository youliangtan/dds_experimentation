// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorldSubscriber.cpp
 *
 */

#include "HelloWorldSubscriber.h"
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include <fastdds/dds/topic/TopicListener.hpp>

using TopicListener = eprosima::fastdds::dds::TopicListener;

//============================================================================

class CustomTopicListener : public TopicListener
{
public:

    CustomTopicListener()
        : TopicListener()
    {
        std::cout << " create custom topic listener" << std::endl;
    }

    //// Not working
    virtual void on_inconsistent_topic(
            eprosima::fastdds::dds::Topic* topic,
            eprosima::fastdds::dds::InconsistentTopicStatus status)
    {
        (void)topic, (void)status;
        std::cout << " Inconsistent topic received discovered" << std::endl;
    }
};

//============================================================================

using namespace eprosima::fastdds::dds;

HelloWorldSubscriber::HelloWorldSubscriber()
    : participant_(nullptr)
    , subscriber_(nullptr)
    , topic_(nullptr)
    , reader_(nullptr)
    // , type_(new HelloWorld_v2PubSubType()) //// Note(YL): Switching incompatible datatype
    , type_(new HelloWorld_v1PubSubType())
{
}

bool HelloWorldSubscriber::init()
{
    DomainParticipantQos pqos;
    pqos.name("Participant_sub");
    participant_ = DomainParticipantFactory::get_instance()->create_participant(0, pqos);

    if (participant_ == nullptr)
    {
        return false;
    }

    //REGISTER THE TYPE
    type_.register_type(participant_);

    //CREATE THE SUBSCRIBER
    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

    if (subscriber_ == nullptr)
    {
        return false;
    }

    std::cout << " Print data type: " << type_.get_type_name() << std::endl;
    CustomTopicListener custom_listener;

    //CREATE THE TOPIC
    topic_ = participant_->create_topic(
        "HelloWorldTopic",
        type_.get_type_name(),
        TOPIC_QOS_DEFAULT,
        &custom_listener
        );

    if (topic_ == nullptr)
    {
        return false;
    }

    // CREATE THE READER
    DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
    rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    reader_ = subscriber_->create_datareader(topic_, rqos, &listener_);

    listener_.listener_participant_ = participant_;

    if (reader_ == nullptr)
    {
        return false;
    }

    return true;
}

HelloWorldSubscriber::~HelloWorldSubscriber()
{
    if (reader_ != nullptr)
    {
        subscriber_->delete_datareader(reader_);
    }
    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
    if (subscriber_ != nullptr)
    {
        participant_->delete_subscriber(subscriber_);
    }
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

void HelloWorldSubscriber::SubListener::on_subscription_matched(
        DataReader* reader,
        const SubscriptionMatchedStatus& info)
{
    if (info.current_count_change == 1)
    {
        matched_ = info.total_count;
        std::cout << "Subscriber matched." << std::endl;

        /// TODO: (YL) test implementation of user data
        std::cout << "--- detected participants: " << std::endl;
        const auto parts = listener_participant_->get_participant_names();
        for (auto &p: parts)
            std::cout << p << std::endl;

        // const auto user_datas = 
        std::vector<InstanceHandle_t> test_handles;
        listener_participant_->get_discovered_participants(test_handles);
        std::cout << "--- Handle participant sizes " 
                  << test_handles.size() << std::endl;

        InstanceHandle_t no_handle;
        eprosima::fastdds::dds::builtin::ParticipantBuiltinTopicData participants_data;
        listener_participant_->get_discovered_participant_data(participants_data, no_handle);
        const auto vec = participants_data.user_data.data_vec();
        for (auto &x: vec)
            std::cout << x << std::endl;
        std::cout << "--> Finished printing part user data of size: " << vec.size() << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        matched_ = info.total_count;
        std::cout << "Subscriber unmatched." << std::endl;
    }
    else
    {
        std::cout << info.current_count_change
                  << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
    }
}

void HelloWorldSubscriber::SubListener::on_data_available(
        DataReader* reader)
{
    SampleInfo info;
    /// Note(YL): Switching incompatible datatype
    HelloWorld_v2 msg;
    // HelloWorld_v1 msg;

    //// Note: experimentation
    {
        InstanceHandle_t no_handle;
        eprosima::fastdds::dds::builtin::PublicationBuiltinTopicData publications;
        reader->get_matched_publication_data(publications, no_handle);
        const auto vec2 = publications.user_data.data_vec();
        for (auto &x: vec2)
            std::cout << x << std::endl;
        std::cout << "--> Finished printing pub user data of size: " << vec2.size() << std::endl;
    }

    if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK)
    {
        if (info.instance_state == ALIVE_INSTANCE_STATE)
        {
            samples_++;
            // Print your structure data here.
            std::cout << "Message " << msg.message() << " " << " RECEIVED" << std::endl;
        }
    }
}

void HelloWorldSubscriber::run()
{
    std::cout << "Subscriber running. Please press enter to stop the Subscriber" << std::endl;
    std::cin.ignore();
}

void HelloWorldSubscriber::run(
        uint32_t number)
{
    std::cout << "Subscriber running until " << number << "samples have been received" << std::endl;
    while (number > listener_.samples_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
