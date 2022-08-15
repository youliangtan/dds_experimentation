#define MSG_VERSION 1 /// Toggle this

#include "dds/dds.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if MSG_VERSION == 1
  #include "HelloWorld_v1.h"
  #define HelloWorld_VX_Msg_desc HelloWorld_v1_Msg_desc
  #define HelloWorld_VX_Msg__alloc HelloWorld_v1_Msg__alloc
  #define HelloWorld_VX_Msg HelloWorld_v1_Msg
  #define HelloWorld_VX_Msg_free HelloWorld_v1_Msg_free
#elif MSG_VERSION == 2
  #include "HelloWorld_v2.h"
  #define HelloWorld_VX_Msg_desc HelloWorld_v2_Msg_desc
  #define HelloWorld_VX_Msg__alloc HelloWorld_v2_Msg__alloc
  #define HelloWorld_VX_Msg HelloWorld_v2_Msg
  #define HelloWorld_VX_Msg_free HelloWorld_v2_Msg_free
#endif

/* An array of one message (aka sample in dds terms) will be used. */
#define MAX_SAMPLES 1

static void inconsistent_topic_cb(dds_entity_t topic, const dds_inconsistent_topic_status_t status, void* arg)
{
    /* do stuff and things with the event */
  printf("calling inconsistent!\n");
  fflush(stdout);
}

static void data_available_handler (dds_entity_t reader, void *arg)
{

  printf("data avail\n");
  fflush(stdout);

//  (void)arg;
//  (void) do_take (reader);
}

int main (int argc, char ** argv)
{
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t reader;
  #if MSG_VERSION == 1
    HelloWorld_v1_Msg *msg;
  #elif MSG_VERSION == 2
    HelloWorld_v2_Msg *msg;
  #elif MSG_VERSION == 3
    HelloWorld_v3_Msg *msg;
  #endif

  void *samples[MAX_SAMPLES];
  dds_sample_info_t infos[MAX_SAMPLES];
  dds_return_t rc;
  dds_qos_t *qos;
  (void)argc;
  (void)argv;

  /* create listener*/
  dds_listener_t* listener;
  listener = dds_listener_create(NULL);
  dds_lset_inconsistent_topic(listener, inconsistent_topic_cb);
  dds_lset_data_available(listener, data_available_handler);

  /* Create a Participant. */
  participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, listener);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  /* Create a Topic. */
  topic = dds_create_topic (
    participant, &HelloWorld_VX_Msg_desc, "hello_world_topic", NULL, listener);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a reliable Reader. */
  qos = dds_create_qos ();
  dds_qset_reliability (qos, DDS_RELIABILITY_RELIABLE, DDS_SECS (10));
  reader = dds_create_reader (participant, topic, qos, listener);
  if (reader < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));
  dds_delete_qos(qos);

  printf ("\n=== [Subscriber] Waiting for a sample ...\n");
  fflush (stdout);

  /* Initialize sample buffer, by pointing the void pointer within
   * the buffer array to a valid sample memory location. */
  samples[0] = HelloWorld_VX_Msg__alloc ();

  /* Poll until data has been read. */
  while (true)
  {
    /* Do the actual read.
     * The return value contains the number of read samples. */
    rc = dds_read (reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
    if (rc < 0)
      DDS_FATAL("dds_read: %s\n", dds_strretcode(-rc));

    /* Check if we read some data and it is valid. */
    if ((rc > 0) && (infos[0].valid_data))
    {
      /* Print Message. */
      msg = (HelloWorld_VX_Msg*) samples[0];
      printf ("=== [Subscriber] Received : ");
      printf ("Message (%"PRId32", %s)\n", msg->index, msg->message);
      fflush (stdout);
      break;
    }
    else
    {
      /* Polling sleep. */
      dds_sleepfor (DDS_MSECS (20));
    }
  }

  /* Free the data location. */
  HelloWorld_VX_Msg_free (samples[0], DDS_FREE_ALL);

  /* Deleting the participant will delete all its children recursively as well. */
  rc = dds_delete (participant);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  return EXIT_SUCCESS;
}
