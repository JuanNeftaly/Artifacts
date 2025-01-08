import { useEffect, useState } from "react";
import { useMqtt } from "../hooks/UseMqtt";

function ButtonPublishForm({ Topic }) {

	const { publish } = useMqtt();

	const [statusToSend, setStatusToSend] = useState(1);

	const handlePublish = (e) => {
		e.preventDefault();

		publish(Topic, statusToSend.toString());

		let value = statusToSend;
		setStatusToSend(value === 0 ? 1 : 0);
	}

	return (
		<button
			onClick={handlePublish}
			className={`p-4 rounded-md transition duration-300 ease-in-out ${statusToSend === 0 ? 'bg-red-500 hover:bg-green-700' : 'bg-green-500 hover:bg-red-700'
				} flex flex-col justify-center items-center font-bold text-lg`}
		>
			{statusToSend === 1 ? 'Encender' : 'Apagar'}
		</button>
	);
}

export default ButtonPublishForm;