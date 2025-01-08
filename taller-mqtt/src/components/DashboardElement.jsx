function DashboardElement({ title, value, unit }) {
  //title: Título del elemento
  //value: Valor del elemento
  //unit: Unidad de medida del elemento
  return (
    <div className="p-4 rounded-md bg-slate-50 text-neutral-800 flex flex-col gap-4justify-center items-center">
      <h2 className="font-bold text-lg">{title}</h2>
      <p className="text-2xl">{value} {unit}</p>
    </div>
  );
}

export default DashboardElement;
