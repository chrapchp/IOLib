# IOLib

A set of help classes to frame my home hydroponic development

Class/Type | Description |
------- | ---------------- | :
DA_IO  | Data Acquisition IO Types
DA_Input| Base class for Discrete & Analog Inputs
DA_DiscreteInput | Discrete Input with debounce, poll rates, edge detections
DA_HOASwitch | Hand-Off-Auto Switch Input
DA_Output | Base class for Discrete * Analog Outputs
DA_Discrete_Output | Basic digital Output
DA_DiscreteOutputTmr | Timer using DA_DiscreteOutput
DA_PeristalticPump | Peristaltic pump using DA_DiscreteOutputTmr
DA_NonBlockingDelay | delays without blocking with callBack
DA_Atlas | Atlas Scientific sensor
DA_AtlasEC | Electrical Conductivity
DA_AtlasPH | pH
DA_AnalogInput | Analog Input with deadband, call back on change  
DA_AnalogOutput | simple analog output
DA_OneWireDallasMgr | 1-wire temperature sensor manager
