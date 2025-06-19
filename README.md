# 🌾 Kissan-Dost: Simplifying IoT Adoption through Conversational AI Framework for Precision Agriculture in Developing Regions

**“Kissan-Dost”** (meaning *Farmer’s Friend*) is a field-validated, end-to-end **IoT + LLM-based conversational system** designed to bridge the gap between smart farming technologies and **low-literate, low-resource smallholder farmers**. It enables actionable agricultural insights through a WhatsApp-based chatbot that speaks the user’s local language—supported by real-time sensor data, weather forecasts, market prices and RAG (retrieval-augmented generation) techniques.

---

## Key Highlights

- **Multilingual** chatbot interface (Urdu, Punjabi, Sindhi) for farmers
- **Voice + Text** over WhatsApp using OpenAI Assistants API
- **Real-time IoT data** from custom field-deployed sensor modules
- **Retrieval-Augmented Generation (RAG)** for crop-specific advisories
- **Low-power sensing gateway** + 4G/ESP-NOW
- Validated over **two 45-day deployments** in Punjab, Pakistan

---

## Research Motivation

Despite IoT’s potential in agriculture, adoption in developing regions is hindered by:

- Infrastructural gaps
- Low literacy rates
- Technological complexity

Kissan-Dost addresses this gap by localizing **LLM-powered agriculture advisory** into familiar communication platforms like WhatsApp, and grounding responses in **real-time sensor data** and regionally tailored agronomic knowledge.

---

## System Architecture

![System Architecture](images/KissanDost_Arch.png)

Kissan-Dost is composed of:

- **Data Collection**: Soil sensors (pH, NPK, moisture, EC, temperature)
- **Wireless Gateway**: ESP32 + LTE backhaul
- **Cloud Orchestration**: LLM agents for query analysis, translation, updates and response synthesis
- **Knowledge Sources**: Real-time weather, AMIS market prices, curated agronomic docs
- **LLM Interface**: Delivered via WhatsApp using OpenAI's Assistants API

---

## Hardware Deployment

![Sensor Module](images/sensor_module.png)

![Farm Deployments](images/Farms.png)

The IoT hardware stack includes:

- ESP32-powered sensor module with a **7-in-1 RS485 sensor suite** (pH, EC, NPK, Temp, Moisture)
- Custom-designed PCB with:
  - TTL converter for RS485
  - Buck converter for regulated 5V output
  - 12V rechargeable battery
- **Sleep-mode logic** for low power operation
- Field-ready **IP65 enclosures** with mounts for crop-level deployment

> 📁 Arduino source code and circuit diagrams are included in [`arduino_scripts/`](./arduino_scripts/) for replication or extension.

These modules were deployed on two test sites (sugarcane and spinach farms), each with an independent sensing + connectivity pipeline.

---

## User Interaction & Data Flow

![Flow Diagram](images/flow_diagram.png)

The system follows a dual-pipeline:

1. **User Flow**:
   - WhatsApp query → Translate → Analyze → Retrieve Data → Generate Response → Translate → Reply

2. **Sensor Flow**:
   - Sensor → Gateway → Cloud → DB → Analyzer → Advice Trigger

---

## Interaction Examples: Chat + Dashboard

Kissan-Dost offers two primary interfaces, which were evaluated in separate phases:

- The ability to monitor field data via a live **web dashboard**
- The **WhatsApp** based chatbot using voice/text in their local languages

<div align="center">

<img src="images/dash-graph.png" alt="Sensor Dashboard" width="45%"/>
<img src="images/whatsapp_example.png" alt="WhatsApp Chat" width="45%" style="margin-left: 20px;"/>


</div>

## Research Contributions

- **IoT-Integrated Conversational AI**: Seamless fusion of real-time soil sensor data with LLM responses via a multilingual WhatsApp interface.
- **Culturally Grounded Design**: End-to-end localization in voice, script and agricultural content across Urdu, Punjabi, and Sindhi.
- **Systematic Technical Evaluation**:
  - **LLM-as-a-Judge** for assessing response accuracy and helpfulness
  - **RAGAS** to measure factuality and relevance in retrieval-augmented responses
  - **COMETKiwi** and **MetricX-24** for reference-free evaluation of translation fidelity

---

## Field Deployment Outcomes

- **Duration**: Two deployments × 45 days (Sugarcane + Spinach)
- **Sites**: 1 commercial farm, 1 university farm
- **Participants**: 5 farmers (managers + field workers)

### Key Findings
- **Limited dashboard adoption**: Farmers exhibited minimal engagement with visual dashboards due to literacy and usability barriers.
- **High chatbot utility**: The WhatsApp-based interface enabled consistent and trusted interaction, particularly through voice.
- **Increased trust and uptake**: Repeated accurate advisories led to growing reliance on the system for daily decision-making.