#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "MQTTPacket.h"
#include "MQTTAPI.H"
#include "transport.h"


// this is a PROVISIONAL declaration, for using topic trace temporally
extern int		tprintf(int hcon, char *texto,...);



#define BUFSIZE	256


int		MqttConnectB(int handle, char *username, char *password) {
	int len;
	int rc;
	unsigned char buf[BUFSIZE];
	char sclientId[12];

	int buflen = BUFSIZE;



	int traza = 0;
	long  tt;

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;


//	tt  = HAL_GetTick();
	tt =  GetTimeStamp();
	srand(tt);
	itoa (rand(), sclientId, 10);

	data.clientID.cstring = sclientId;
	data.keepAliveInterval = 300;
	data.cleansession = 1;
	data.username.cstring = username;
	data.password.cstring = password;
//	data.MQTTVersion = 4;  // default
//	data.MQTTVersion = 3;



	/////////////// (2)  SERIALIZA para un CONNECT  (PPT-2-a)
	len = MQTTSerialize_connect(buf, buflen, &data);
	///////////////     ENVIA						(PPT-2-b)
	rc = transport_sendPacketBuffer(handle, buf, len);

	/* wait for connack */
	/////////////// (4)  CHECK, para ver si es un CONNACK (PPT-3-a)

	if (rc && (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK))
	{
		unsigned char sessionPresent, connack_rc;

		/////////////// (5)  DESERIALIZA el CONNACK y verifica que sea ACK ok  (PPT-3-b)
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			if (traza) printf("Unable to connect, return code %d\n", connack_rc);
			return (-(100+rc));
//			return -1;
		}
		return handle;
	}
	else {
		return -1;
	}


}



int		MqttConnect(const char	*host, unsigned int port, int security, char *username, char *password) {

	int handle = transport_open(host, port, security);
 //	handle = M95_ReConnect(host, port);
	if (handle > 0)
		 MqttConnectB(handle, username, password);
	return  handle;
}



int		MqttSubscribe (int handle, char *topic){
	int	traza = 0;
	MQTTString topicString = MQTTString_initializer;
	unsigned char buf[BUFSIZE];
	int buflen = BUFSIZE;
	unsigned short msgid = 1;
	int req_qos = 0;
	int rc; 
	int len;
	topicString.cstring = topic;
	if (traza) tprintf(handle, "Trying to subscribe  %s\n", topic);
	/////////////// (5)  SERIALIZA para un "SUSCRIBE TOPIC"	PPT(4-a)
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
	/////////////// (6)  ENVIA Mssg SUSCRIPCION		PPT(4-b)
	rc = transport_sendPacketBuffer(handle, buf, len);
	/////////////// (7)  CHECK, para ver si es un SUBACK (PPT-5a)
	rc = MQTTPacket_read(buf, buflen, transport_getdata); 
	/* check for suback */
	if (rc == SUBACK) {
		unsigned short submsgid;
		int subcount;
		int granted_qos;
		/////////////// (8)  DESERIALIZA el SUBACK  para obtener esas tres cosas ?? (PPT-5-b)
		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos == 0){
			if (traza) tprintf(handle, "Subscription OK!!!\n");
			return 1;
		}else {
			if (traza) tprintf(handle, "granted qos != 0, %d\n", granted_qos);
			return -1;
		}
	}
	else {
		if (traza) tprintf(handle, "Failure expecting SUBACK, got %d\n", rc);

	}
		return -1;

}

///////////////////
// IMPORTANTE!!!
// el payload se pasa como un puntero a puntero para que 
//		el DeserializePublish lo reasigne a la zona del buf donde est�
// O sea lo que obtengo en payload es una parte interna del buf
//	Si el buf es local, hay que copiarlo o duplicarlo
extern	char	*MqttGetMessage(int handle, char *_destination, int maxlen) {
	int	traza = 0;
	unsigned char	buf[BUFSIZE];
//	MQTTString topicString = MQTTString_initializer;
	int myrc;
	if (traza) tprintf (handle, "Before PacketRead");
	myrc = MQTTPacket_read(buf, maxlen, transport_getdata);
	if (traza) tprintf (handle, "PacketRead got %d rc", myrc);

	if (myrc == PUBLISH) {
		unsigned char dup;
		int qos;
		unsigned char retained;
		unsigned short msgid;
		int payloadlen_in;
//		unsigned char* payload_in;
		unsigned char	*p=NULL;
		int rc;
		MQTTString receivedTopic;

		/////////////// Deserializa lo que haya entrado para ver los FLAGS!!! (PPT 6-b)
		if (traza) tprintf (handle, "go to Deserialize on buf");
		rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
				&p, &payloadlen_in, buf, maxlen);
//		if (1) tprintf (handle, "returned form Deserialize on buf", myrc);
		if (rc == 1 && p!= NULL) {
			if (0)  {
				tprintf(handle, "Payload begin in after %d \n", p - buf);
				tprintf(handle, "Arrived a message on TOPIC #%.*s# \n", receivedTopic.lenstring.len, receivedTopic.lenstring.data);
				tprintf(handle, "Payload  %.*s\n",payloadlen_in, p );
			}
			if (p) {
				// getch();
				p[payloadlen_in] = 0;
				strcpy (_destination, (char *) p);
				if (traza) tprintf (handle, "Ready to deliver message");
				return _destination;
			}
			else {
				if (1) tprintf (handle, "ERROR: Deserialization done, but no payload reported; returning NULL\n");
				return NULL;
			}
		}
		else {
			if (traza) tprintf (handle, "ERROR: Faillure in MQTTDeserialize, returning NULL\n");
			return NULL;
		}
	}
	else {
		if (traza) tprintf (handle, "WARNING: Got %d when expecting PUBLISH. Returning NULL\n", myrc);
		return NULL;
	}
}




int		MqttPutMessage(int handle, char	*topic, char *message){
	/////////////// (*)  PUBLICA LO QUE SEA  (PPT missing)
	int rc;
	int traza = 0;  // has to be 0 as fas as calls tprintf, because if not, it recurses without end
	MQTTString topicString = MQTTString_initializer;
	unsigned char	buf[BUFSIZE];
	int		buflen = BUFSIZE;
	int		len;
	topicString.cstring = topic;

	if (traza) tprintf(handle, "To Publish %s in topic %s \n", message, topic);
	len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)message, strlen(message));
	if (traza) tprintf(handle, "Message serialized to  %d bytes \n", len);
	rc = transport_sendPacketBuffer(handle, buf, len);
	if (traza) tprintf(handle, "Message sended with %d RC \n", rc);
	return rc;
}


int		MqttCheck(int handle){
	return handle;
}



int		MqttDisconnectB(int handle){
	int len;
	unsigned char	buf[BUFSIZE];
	int		buflen = BUFSIZE;
	int		rc;
	int		traza = 0;

	if (traza) tprintf(handle, "disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(handle, buf, len);

	return rc;
}

int		MqttDisconnect(int handle){
	int rc = MqttDisconnectB(handle);
	if (rc)
		transport_close(handle);
	return rc;
}


int		MqttReConnect		(int handle, char *host, int port, int security, char *username, char *password) {
	int len;
	unsigned char	buf[BUFSIZE];
	int		buflen = BUFSIZE;
	int		traza = 0;
	int rc;

//	rc = MqttDisconnectB(handle);
	if (1) {
		handle = transport_reopen(host, port, security);
		if (handle > 0) {
			handle = MqttConnectB(handle, username, password);
		}
		return handle;
	}


}


int			MqttPing(int handle){
	/////////////// (*)  Sending PingReq, and expecting PingResp
	int rc;
	int	buflen = 64;
	unsigned char	buf[64];
	int rem_len = 0;
	int len;
	int traza = 0;  // has to be 0 as fas as calls tprintf, because if not, it recurses without end
	if (traza) tprintf(handle, "Going to pingreq  \n");
	len = MQTTSerialize_pingreq(buf, buflen);
	if (traza) tprintf(handle, "Deserialized to %d\n", len);
	rc = transport_sendPacketBuffer(handle, buf, len);
	if (traza) tprintf(handle, "PING sended with %d RC \n", rc);
	if (rc) {
		MQTTHeader header = {0};
		if (transport_getdata(buf, 1) == 1){
			header.byte = buf[0];
			if (header.bits.type == PINGRESP) {
				if (traza) tprintf(handle, "ooooKKKK\n");
				len = 1;
				/* 2. read the remaining length.  Suposed ti be 0 */
				MQTTPacket_decode(transport_getdata, &rem_len);
				if (rem_len == 0)
					rc = 1;
				else
					rc = 0;
			}
			else
				rc = 0;
		}
		else
			rc = 0;
	}
	else {
		if (traza) tprintf(handle, "NOOOOooooKKKK %D\n", rc);
	}

	return rc;
}



// Helper function to publish-out trace messages
int		tprintf(int hcon, char *texto,...) {
	int rc;
	va_list	  ap;
	char	  salida[512];

	va_start	  (ap, texto);
//	sprintf (salida, "Node %s :", GetVariable("ID"));
//	sprintf (salida, "Node %s <%s> ", GetVariable("ID"), strDateTime());
	vsprintf (salida, texto, ap);
	rc = MqttPutMessage	(hcon, topictr, salida);
	return rc;
}

