/**
 * MQTT Broker Lambda Function
 * Processes messages from IoT devices and performs routing/transformation
 */

export interface MeteoRaysEvent {
  topic: string;
  payload: any;
  timestamp: number;
}

export interface LightningEvent {
  type: 'lightning';
  distance: number;
  energy: number;
  lat?: number;
  lon?: number;
  timestamp?: number;
}

export interface LocationEvent {
  lat: number;
  lon: number;
  alt: number;
  sats: number;
  timestamp?: number;
}

export const handler = async (event: any): Promise<any> => {
  console.log('Received IoT message:', JSON.stringify(event, null, 2));

  try {
    const topic = event.topic || '';
    const payload = typeof event === 'string' ? JSON.parse(event) : event;

    // Add timestamp if not present
    const timestamp = payload.timestamp || Date.now();

    // Process based on topic
    if (topic.includes('lightning')) {
      return await processLightningEvent({ ...payload, timestamp });
    } else if (topic.includes('location')) {
      return await processLocationEvent({ ...payload, timestamp });
    } else {
      console.log('Unknown topic, logging only:', topic);
      return {
        statusCode: 200,
        body: JSON.stringify({ message: 'Message logged' }),
      };
    }
  } catch (error) {
    console.error('Error processing message:', error);
    return {
      statusCode: 500,
      body: JSON.stringify({ error: 'Failed to process message' }),
    };
  }
};

async function processLightningEvent(event: LightningEvent): Promise<any> {
  console.log('Processing lightning event:', event);

  // Here you could:
  // - Store in DynamoDB
  // - Send notifications via SNS
  // - Trigger additional workflows
  // - Forward to other systems

  const enrichedEvent = {
    ...event,
    severity: calculateSeverity(event.distance),
    processed_at: new Date().toISOString(),
  };

  console.log('Enriched lightning event:', enrichedEvent);

  return {
    statusCode: 200,
    body: JSON.stringify({
      message: 'Lightning event processed',
      data: enrichedEvent,
    }),
  };
}

async function processLocationEvent(event: LocationEvent): Promise<any> {
  console.log('Processing location event:', event);

  // Here you could:
  // - Update device location in database
  // - Trigger geofencing rules
  // - Store location history

  const enrichedEvent = {
    ...event,
    processed_at: new Date().toISOString(),
  };

  console.log('Enriched location event:', enrichedEvent);

  return {
    statusCode: 200,
    body: JSON.stringify({
      message: 'Location event processed',
      data: enrichedEvent,
    }),
  };
}

function calculateSeverity(distance: number): string {
  if (distance <= 5) {
    return 'critical';
  } else if (distance <= 10) {
    return 'high';
  } else if (distance <= 20) {
    return 'medium';
  } else {
    return 'low';
  }
}
