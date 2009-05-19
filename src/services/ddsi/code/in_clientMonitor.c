/* TODO replace printf-error messaging by REPORT logging */
#include "in_clientMonitor.h"

#include "u_user.h"
#include "u_participant.h"
#include "u_subscriber.h"
#include "u_dataReader.h"
#include "u_waitset.h"
#include "u_waitsetEvent.h"
#include "c_iterator.h"
#include "v_event.h"
#include "v_readerSample.h"
#include "v_dataReaderSample.h"
#include "v_public.h"
#include "v_topic.h"
#include "v_builtin.h"
#include "v_state.h"
#include "v_dataReaderInstance.h"

#include "in_runnable.h"
#include "in_report.h"


static u_result
in_clientMonitorAttachAndMonitor(
	in_clientMonitor self,
    const u_dataReader participantReader,
    const u_dataReader publicationReader,
    const u_dataReader subscriptionReader);

static u_result
in_clientMonitorStartMonitoring(
	in_clientMonitor self,
    const u_waitset waitset,
    const u_dataReader participantReader,
    const u_dataReader publicationReader,
    const u_dataReader subscriptionReader);

static c_bool
takeOne(
    c_object o,
    c_voidp arg);

static u_result
in_clientMonitorHandleParticipant(
	in_clientMonitor self,
    u_dataReader dataReader,
    c_long dataOffset);

static u_result
in_clientMonitorHandleSubscription(
	in_clientMonitor self,
    u_dataReader dataReader,
    c_long dataOffset);

static u_result
in_clientMonitorHandlePublication(
	in_clientMonitor self,
    u_dataReader dataReader,
    c_long dataOffset);

static u_result
in_clientMonintorHandleInterrupt(
	in_clientMonitor self);

static void
resolveOffset(
    v_entity e,
    c_voidp arg);


void
in_clientMonitorInit(
		in_clientMonitor self,
		in_runnable runnable,
		u_participant participant,
		c_time periodic,
		in_clientMonitorParticipantAction   participantAction,
		in_clientMonitorSubscriptionAction  subscriptionAction,
		in_clientMonitorPublicationAction   publicationAction,
		in_clientMonitorPeriodicAction      periodicAction)
{
	self->runnable = runnable;
	self->participant = participant;
	self->periodic = periodic;
	self->participantAction = participantAction;
	self->subscriptionAction = subscriptionAction;
	self->publicationAction = publicationAction;
	self->periodicAction = periodicAction;
}

void
in_clientMonitorDeinit(in_clientMonitor self)
{
	/* nop */
}


void
in_clientMonitorTrigger(in_clientMonitor self)
{

}

os_boolean
in_clientMonitorRun(in_clientMonitor self)
{
    os_boolean result = TRUE;
    u_subscriber subscriber;
    u_participant participant = NULL;
    c_iter readers;
    int length;
    u_result uresult;
    u_dataReader participantReader, subscriptionReader, publicationReader;


	participant = self->participant;

	if(participant){
		/* Gain access to the built-in subscriber.*/
		subscriber = u_participantGetBuiltinSubscriber(participant);
		if(subscriber){
			/*Gain access to the built-in participant reader.*/
			readers = u_subscriberLookupReaders(subscriber, V_PARTICIPANTINFO_NAME);
			length  = c_iterLength(readers);

			if(length == 1){
				participantReader = (u_dataReader)c_iterTakeFirst(readers);
			} else {
				printf("Could not resolve built-in participant reader.\n");
				result = FALSE;
			}
			c_iterFree(readers);

			/*Gain access to the built-in subscription reader.*/
			readers = u_subscriberLookupReaders(subscriber, V_SUBSCRIPTIONINFO_NAME);
			length  = c_iterLength(readers);

			if(length == 1){
				subscriptionReader = (u_dataReader)c_iterTakeFirst(readers);
			} else {
				printf("Could not resolve built-in subscription reader.\n");
				result = FALSE;
			}
			c_iterFree(readers);

			/*Gain access to the built-in publication reader.*/
			readers = u_subscriberLookupReaders(subscriber, V_PUBLICATIONINFO_NAME);
			length  = c_iterLength(readers);

			if(length == 1){
				publicationReader = (u_dataReader)c_iterTakeFirst(readers);
			} else {
				printf("Could not resolve built-in publication reader.\n");
				result = FALSE;
			}
			c_iterFree(readers);

			if(result == TRUE){
				uresult = in_clientMonitorAttachAndMonitor(
						self,
						participantReader,
						publicationReader,
						subscriptionReader);

				if((uresult != U_RESULT_OK) &&
				   (uresult != U_RESULT_DETACHING))
				{
					printf("Abnormal termination...\n");
					result = FALSE;
				} else {
					printf("Deleting entities...\n");
				}
			}
			/*Delete datareaders*/
			uresult = u_dataReaderFree(participantReader);

			if(uresult != U_RESULT_OK){
				printf("Deletion of participant reader failed.\n");
				result = FALSE;
			}
			uresult = u_dataReaderFree(subscriptionReader);

			if(uresult != U_RESULT_OK){
				printf("Deletion of subscription reader failed.\n");
				result = FALSE;
			}
			uresult = u_dataReaderFree(publicationReader);

			if(uresult != U_RESULT_OK){
				printf("Deletion of publication reader failed.\n");
				result = FALSE;
			}
			/*Delete subscriber*/
			uresult = u_subscriberFree(subscriber);

			if(uresult != U_RESULT_OK){
				printf("Deletion of subscriber failed.\n");
				result = FALSE;
			}
		} else {
			printf("Could not create subscriber.\n");
			result = FALSE;
		}
	} else {
		printf("Could not create participant.\n");
		result = FALSE;
	}
    return result;
}

static void
resolveOffset(
    v_entity e,
    c_voidp arg)
{
    c_long* offset;

    offset = (c_long*)arg;
    *offset = v_topic(e)->dataField->offset;

    return;
}

static u_result
in_clientMonitorAttachAndMonitor(
	in_clientMonitor self,
    const u_dataReader participantReader,
    const u_dataReader publicationReader,
    const u_dataReader subscriptionReader)
{
    u_waitset waitset;
    u_dataReader dataReader;
    c_iter readers;
    u_result result;
    c_long i, length;

    u_participant participant = self->participant;

    result = U_RESULT_INTERNAL_ERROR;

    /*Create waitset.*/
    waitset = u_waitsetNew(participant);

    if(waitset){
        /*Set event mask of the waitset.*/
        result = u_waitsetSetEventMask(waitset, V_EVENT_DATA_AVAILABLE);

        if(result == U_RESULT_OK){
            readers     = c_iterNew(participantReader);
            readers     = c_iterInsert(readers, publicationReader);
            readers     = c_iterInsert(readers, subscriptionReader);

            result     = U_RESULT_OK;
            length     = c_iterLength(readers);

            for(i=0; i<length && (result == U_RESULT_OK); i++){
                dataReader = (u_dataReader)(c_iterObject(readers, i));

                /*Set event mask of the datareader to trigger on available data.*/
                result = u_dispatcherSetEventMask(
                            (u_dispatcher)dataReader, V_EVENT_DATA_AVAILABLE);

                if(result == U_RESULT_OK){
                    /*Attach reader to the waitset.*/
                    result = u_waitsetAttach(
                            waitset, (u_entity)dataReader, (u_entity)dataReader);

                    if(result != U_RESULT_OK){
                        printf("Could not attach datareader to waitset.\n");
                    }
                } else {
                    printf("Could not set event mask of datareader.\n");
                }
            }
        } else {
            printf("Could not set event mask of waitset.\n");
        }


        if(result == U_RESULT_OK){
            /*Start monitoring the creation/deletion of entities.*/
            result = in_clientMonitorStartMonitoring(
                                self, waitset, participantReader,
                                publicationReader, subscriptionReader);
        }
        /*Detach all datareaders from the waitset.*/
        for(i=0; i<length; i++){
            u_waitsetDetach(waitset, (u_entity)(c_iterObject(readers, i)));
        }
        c_iterFree(readers);
        /*Delete the waitset.*/
        result = u_waitsetFree(waitset);

        if(result != U_RESULT_OK){
            printf("Deletion of waitset failed.\n");
        }
    } else {
        printf("Could not create waitset.\n");
    }

    return result;
}

static u_result
in_clientMonitorStartMonitoring(
	in_clientMonitor self,
    const u_waitset waitset,
    const u_dataReader participantReader,
    const u_dataReader publicationReader,
    const u_dataReader subscriptionReader)
{
    os_boolean terminate = OS_FALSE;
    c_iter events, topics;
    u_waitsetEvent event;
    c_time timeout;
    v_gid participantGid, publicationGid, subscriptionGid, gid;
    u_result result;
    u_dataReader dataReader;
    u_topic topic;
    v_duration duration;
    c_long participantOffset, publicationOffset, subscriptionOffset;

    u_participant participant = self->participant;

    /*Resolve unique identifications of readers*/
    participantGid  = u_entityGid((u_entity)participantReader);
    publicationGid  = u_entityGid((u_entity)publicationReader);
    subscriptionGid = u_entityGid((u_entity)subscriptionReader);

    /*Resolve topics to find offsets in the data. The offsets are used later on*/
    duration.seconds = 0;
    duration.nanoseconds = 0;

    topics = u_participantFindTopic(participant, V_PARTICIPANTINFO_NAME, duration);
    topic  = c_iterTakeFirst(topics);

    if(topic){
        result = u_entityAction(u_entity(topic), resolveOffset, &participantOffset);
    } else {
        result = U_RESULT_INTERNAL_ERROR;
        printf("Could not resolve participant info offset.\n");
    }
    c_iterFree(topics);

    if(result == U_RESULT_OK){
        topics = u_participantFindTopic(participant, V_PUBLICATIONINFO_NAME, duration);
        topic  = c_iterTakeFirst(topics);

        if(topic){
            result = u_entityAction(u_entity(topic), resolveOffset, &publicationOffset);
        } else {
            result = U_RESULT_INTERNAL_ERROR;
            printf("Could not resolve publication info offset.\n");
        }
        c_iterFree(topics);
    }

    if(result == U_RESULT_OK){
        topics = u_participantFindTopic(participant, V_SUBSCRIPTIONINFO_NAME, duration);
        topic  = c_iterTakeFirst(topics);

        if(topic){
            result = u_entityAction(u_entity(topic), resolveOffset, &subscriptionOffset);
        } else {
            result = U_RESULT_INTERNAL_ERROR;
            printf("Could not resolve subscription info offset.\n");
        }
        c_iterFree(topics);
    }

    if(result == U_RESULT_OK){
    	timeout = self->periodic;
#if 0
        timeout.seconds     = 1;
        timeout.nanoseconds = 0;
#endif
        printf("Collecting initial entities...\n");
        result = in_clientMonitorHandleParticipant(self,
        		participantReader,
        		participantOffset);

        if(result == U_RESULT_OK){
            result = in_clientMonitorHandlePublication(self,
            		publicationReader,
            		publicationOffset);

            if(result == U_RESULT_OK){
                result = in_clientMonitorHandleSubscription(self,
                		subscriptionReader,
                		subscriptionOffset);

                if(result == U_RESULT_OK){
                    printf("Waiting for entities to be created/deleted...\n");
                } else {
                    printf("Could not collect initial subscriptions...\n");
                }
            } else {
                printf("Could not collect initial publications...\n");
            }
        } else {
            printf("Could not collect initial participants...\n");
        }
    }

	timeout = self->periodic;

    while(!terminate
    	  && !(int)in_runnableTerminationRequested(self->runnable)) {
        events = NULL;
        /*Wait for events to occur*/
        result = u_waitsetTimedWaitEvents(waitset, timeout, &events);
        if (result == U_RESULT_TIMEOUT) {
        	/* Timeout or triggered */
        	in_clientMonintorHandleInterrupt(self);
        }
        /* TODO in case of timeouts call discovery periodic action handler */
        else if(result == U_RESULT_OK){
            event = (u_waitsetEvent)(c_iterTakeFirst(events));

            while(event){
                if(((event->events) & V_EVENT_DATA_AVAILABLE) ==
                    V_EVENT_DATA_AVAILABLE)
                {
                    if(event->entity){
                        dataReader = (u_dataReader)event->entity;
                        gid        = u_entityGid(event->entity);

                        if(v_gidCompare(gid, participantGid) == C_EQ){
                            result = in_clientMonitorHandleParticipant(
                            		self,
                                    participantReader,
                                    participantOffset);
                        } else if(v_gidCompare(gid, subscriptionGid) == C_EQ){
                            result = in_clientMonitorHandleSubscription(
                            		self,
                                    subscriptionReader,
                                    subscriptionOffset);
                        } else if(v_gidCompare(gid, publicationGid) == C_EQ){
                            result = in_clientMonitorHandlePublication(
                            		self,
                                    publicationReader,
                                    publicationOffset);
                        } else {
                            printf("This is impossible.\n");
                            result = U_RESULT_INTERNAL_ERROR;
                        }
                    } else {
                        printf("DATA_AVAILABLE (%d) but no entity.\n",
                                event->events);
                    }
                } else {
                    printf("Received unexpected event %d.\n", event->events);
                    result = U_RESULT_INTERNAL_ERROR;
                }
                u_waitsetEventFree(event);
                event = (u_waitsetEvent)(c_iterTakeFirst(events));
            }
        } else if(result == U_RESULT_DETACHING){
            printf("Starting termination now...\n");
            terminate = OS_TRUE;
        } else {
            printf("Waitset wait failed.\n");
            terminate = OS_TRUE;
        }
        if(events){/* events may be null if waitset was deleted */
            c_iterFree(events);
        }
    }
    return result;
}

static c_bool
takeOne(
    c_object o,
    c_voidp arg)
{
    v_readerSample s;
    v_readerSample *sample;
    c_bool result;

    s      = (v_readerSample)o;
    sample = (v_readerSample *)arg;


    if (s != NULL) {
        result = TRUE;
        if (v_stateTest(s->sampleState, L_VALIDDATA)) {
            *sample = c_keep(s);
            result = FALSE;
        }
    } else { /* last sample */
        result = FALSE;
    }

    return result;
}

static u_result
in_clientMonitorHandleParticipant(
	in_clientMonitor self,
    u_dataReader dataReader,
    c_long dataOffset)
{
    v_dataReaderSample sample = NULL;
    u_result result;
    v_state sampleState;
    v_state instanceState;
    v_message msg;
    v_dataReaderInstance instance = NULL;
    struct v_participantInfo *data;

    result = u_dataReaderTake(dataReader, takeOne, &sample);

    while(sample && (result == U_RESULT_OK)){
        sampleState = v_readerSample(sample)->sampleState;
        instance = v_dataReaderInstance(v_readerSample(sample)->instance);
        instanceState = instance->instanceState;
        msg   = v_dataReaderSampleMessage(sample);
        data  = (struct v_participantInfo *)(C_DISPLACE(msg, dataOffset));
        self->participantAction(self->runnable,
        		sampleState,
                instanceState,
        		msg,
        		data);

        c_free(sample);
        sample = NULL;
        result = u_dataReaderTake(dataReader, takeOne, &sample);
    }
    return result;
}

static u_result
in_clientMonitorHandleSubscription(
	in_clientMonitor self,
    u_dataReader dataReader,
    c_long dataOffset)
{
    v_dataReaderSample sample = NULL;
    u_result result;
    v_state sampleState;
    v_state instanceState;
    v_message msg;
    v_dataReaderInstance instance = NULL;
    struct v_subscriptionInfo *data;

    result = u_dataReaderTake(dataReader, takeOne, &sample);

    while(sample && (result == U_RESULT_OK)){
        os_boolean ignore = OS_FALSE;
        sampleState = v_readerSample(sample)->sampleState;
        instance = v_dataReaderInstance(v_readerSample(sample)->instance);
        instanceState = instance->instanceState;
        msg   = v_dataReaderSampleMessage(sample);
        data  = (struct v_subscriptionInfo *)(C_DISPLACE(msg, dataOffset));
        if(c_arraySize(data->partition.name) == 1 && (0 == strcmp((os_char*)data->partition.name[0], "__BUILT-IN PARTITION__")))
        {
            if((0 == strcmp(data->topic_name, "DCPSTopic")) ||
               (0 == strcmp(data->topic_name, "DCPSPublication")) ||
               (0 == strcmp(data->topic_name, "DCPSParticipant")) ||
               (0 == strcmp(data->topic_name, "DCPSSubscription")) ||
               (0 == strcmp(data->topic_name, "DCPSHeartbeat")))
            {
                ignore = OS_TRUE;
                IN_TRACE_2(Send, 2, "Ignoring topic '%s' on partition '%s' for subscription.", data->topic_name, data->partition.name[0]);
            }
        }
        if(!ignore)
        {
            IN_TRACE_1(Send, 2, "Calling subscription action for topic '%s'.", data->topic_name);
            self->subscriptionAction(self->runnable,
                sampleState,
                instanceState,
                msg,
                data);
        }
        c_free(sample);
        sample = NULL;
        result = u_dataReaderTake(dataReader, takeOne, &sample);
    }
    return result;
}

static u_result
in_clientMonitorHandlePublication(
	in_clientMonitor self,
    u_dataReader dataReader,
    c_long dataOffset)
{
    v_dataReaderSample sample = NULL;
    u_result result;
    v_state sampleState;
    v_dataReaderInstance instance = NULL;
    v_state instanceState;
    v_message msg;
    struct v_publicationInfo *data;

    result = u_dataReaderTake(dataReader, takeOne, &sample);

    while(sample && (result == U_RESULT_OK)){
        os_boolean ignore = OS_FALSE;
        sampleState = v_readerSample(sample)->sampleState;
        instance = v_dataReaderInstance(v_readerSample(sample)->instance);
        instanceState = instance->instanceState;
        msg   = v_dataReaderSampleMessage(sample);
        data  = (struct v_publicationInfo *)(C_DISPLACE(msg, dataOffset));
        if(c_arraySize(data->partition.name) == 1 && (0 == strcmp((os_char*)data->partition.name[0], "__BUILT-IN PARTITION__")))
        {
            if((0 == strcmp(data->topic_name, "DCPSTopic")) ||
               (0 == strcmp(data->topic_name, "DCPSPublication")) ||
               (0 == strcmp(data->topic_name, "DCPSParticipant")) ||
               (0 == strcmp(data->topic_name, "DCPSSubscription")) ||
               (0 == strcmp(data->topic_name, "DCPSHeartbeat")))
            {
                IN_TRACE_2(Send, 2, "Ignoring topic '%s' on partition '%s' for publication.", data->topic_name, data->partition.name[0]);
                ignore = OS_TRUE;
            }
        }
        if(!ignore)
        {
            IN_TRACE_1(Send, 2, "Calling publication action for topic '%s'.", data->topic_name);
            self->publicationAction(self->runnable,
                sampleState,
                instanceState,
                msg,
                data);
        }

        c_free(sample);
        sample = NULL;
        result = u_dataReaderTake(dataReader, takeOne, &sample);
    }
    return result;
}

static u_result
in_clientMonintorHandleInterrupt(
	in_clientMonitor self)
{
	self->periodicAction(self->runnable);
	return U_RESULT_OK;
}