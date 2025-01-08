import { useMqtt } from "../hooks/UseMqtt";
import DashboardElement from "../components/DashboardElement";
import DashboardForm from "../components/DashboardPublishForm";
import ButtonPublishForm from "../components/OnOffButton";

function Dashboard() {
  // Obtener los mensajes de los topics suscritos
  const { messages } = useMqtt();

  return (
    <main className="min-h-[100dvh] h-full bg-slate-900 text-slate-100 flex flex-col justify-center items-center gap-6 p-4">
      <h1 className="font-bold text-xl">Taller MQTT</h1>
      <div className="w-full max-w-2xl flex flex-col gap-10 justify-center items-center">
        {/* Formulario para publicar mensajes */}
        <DashboardForm topic="/test/message" label="Message" />

        {/* Elementos para mostrar los mensajes de los topics suscritos */}
        <div className="flex flex-wrap gap-6 w-full justify-center items-center">

          <DashboardElement title={"Humedad"} value={messages["/sensor/dht_humedad"]} />
          <DashboardElement title={"Temperatura"} value={messages["/sensor/dht_temperatura"]} />
          <DashboardElement title={"BMP"} value={messages["/sensor/bmp"]} />
          <DashboardElement title={"LDR"} value={messages["/sensor/ldr"]} />
          <DashboardElement title={"RAIN"} value={messages["/sensor/rain"]} />

          <ButtonPublishForm Topic={"/test/Led"} />

        </div>
      </div>
    </main>
  );
}

export default Dashboard;
