import { MqttProvider } from "./context/MqttContext";
import Dashboard from "./pages/Dashboard";

function App() {
  const brokerUrl = "ws://127.0.0.1:9001"; // URL de tu broker MQTT -> ws://<IP>:<Puerto>
  const options = {
    username: "", // Usuario MQTT
    password: "", // Contraseña MQTT
  };

  // para suscribirse 
  const topics = ["/test/int", "/test/float", "/test/comment", "/sensor/dht_temperatura", "/sensor/dht_humedad", "/sensor/bmp", "/sensor/ldr", "/sensor/rain"]; // Lista de topics a los que se suscribirá

  return (
    <MqttProvider brokerUrl={brokerUrl} options={options} topics={topics}>
      <Dashboard />
    </MqttProvider>
  );
}

export default App;