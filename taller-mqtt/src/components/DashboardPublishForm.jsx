import { useState } from "react";
import { useMqtt } from "../hooks/UseMqtt";

function DashboardPublishForm({ topic, label }) {
  // Obtener la función publish del contexto MQTT
  const { publish } = useMqtt();
  // Estado para almacenar el mensaje a enviar
  const [messageToSend, setMessageToSend] = useState("");

  // Handler para realizar publish a un topic
  const handlePublish = (e) => {
    e.preventDefault();

    publish(topic, messageToSend);

    setMessageToSend("");
  };

  return (
    <form onSubmit={handlePublish} className="flex gap-6 flex-col w-full max-w-sm">
      <div className="flex flex-col gap-2">
        <label htmlFor="form" className="font-bold">{label}</label>
        <input
          id="form"
          className="text-black p-2 outline-none rounded-md h-full"
          type="text"
          value={messageToSend}
          onChange={(e) => setMessageToSend(e.target.value)}
        />
      </div>

      <button
        type="submit"
        className="bg-slate-400 p-2 rounded-md text-neutral-800"
      >
        Publicar Mensaje
      </button>
    </form>
  );
}

export default DashboardPublishForm;
