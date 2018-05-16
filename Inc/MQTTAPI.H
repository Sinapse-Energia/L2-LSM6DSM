#ifndef MQTTAPI_H_
#define MQTTAPI_H_

//  Full connection from scratch (open transport and then connect to broker)
extern	int		MqttConnect		(const char	*host, unsigned int port, int sec, char *username, char *password);

// Mqtt Broker connection and subscription, over an already available transport layer
extern	int		MqttConnectB	(int handle, char *username, char *password);

extern	int		MqttSubscribe	(int handle, char *topic);

extern	char	*MqttGetMessage	(int handle, char *buf, int bufsize); // fgets style

//extern	int		Subscribe		(int handle, char *topic);

extern	int		MqttPutMessage	(int handle, char *topic, char *message);

extern	int		MqttPing(int handle);


// abbreviated way of closing and opening connection over another url
extern	int		MqttReConnect		(int handle, char *host, int port, int security, char *username, char *password);

// close broker connection only
extern	int		MqttDisconnectB	(int handle);

// Full disconnection (broker disconnection and ordered transport close)
extern	int		MqttDisconnect	(int handle);



extern int		tprintf(int hcon, char *texto,...);



extern	char	*topictr;


#endif

