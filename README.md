# 🌾 Kissan-Dost: Simplifying IoT Adoption through Conversational AI Framework for Precision Agriculture in Developing Regions

**“Kissan-Dost”** (meaning *Farmer’s Friend*) is a field-validated, end-to-end **IoT + LLM-based conversational system** designed to bridge the gap between smart farming technologies and **low-literate, low-resource smallholder farmers**. It enables actionable agricultural insights through a WhatsApp-based chatbot that speaks the user’s local language—supported by real-time sensor data, weather forecasts, market prices and RAG (retrieval-augmented generation) techniques.

---

## 📌 Key Highlights

- **Multilingual** chatbot interface (Urdu, Punjabi, Sindhi) for farmers
- **Voice + Text** over WhatsApp using OpenAI Assistants API
- **Real-time IoT data** from custom field-deployed sensor modules
- **Retrieval-Augmented Generation (RAG)** for crop-specific advisories
- **Low-power sensing gateway** + 4G/ESP-NOW
- Validated over **two 45-day deployments** in Punjab, Pakistan

---

## 🔬 Research Motivation

Despite IoT’s potential in agriculture, adoption in developing regions is hindered by:

- Infrastructural gaps
- Low literacy rates
- Technological complexity

Kissan-Dost addresses this gap by localizing **LLM-powered agriculture advisory** into familiar communication platforms like WhatsApp, and grounding responses in **real-time sensor data** and regionally tailored agronomic knowledge.

---

## 🧠 System Architecture

![System Architecture](images/KissanDost_Arch.png)

Kissan-Dost is composed of:

- **Data Collection**: Soil sensors (pH, NPK, moisture, EC, temperature)
- **Wireless Gateway**: ESP32 + LTE backhaul
- **Cloud Orchestration**: LLM agents for query analysis, translation, updates and response synthesis
- **Knowledge Sources**: Real-time weather, AMIS market prices, curated agronomic docs
- **LLM Interface**: Delivered via WhatsApp using OpenAI's Assistants API

---

## 🛠️ Hardware Deployment

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

## 💬 Conversational Interaction

![WhatsApp Chat](images/whatsapp_example.png)

The system detects intent, retrieves relevant sensor and contextual data and delivers **voice or text-based advice** accordingly.

---

## 🔁 User Interaction & Data Flow

![Flow Diagram](images/flow_diagram.png)

The system follows a dual-pipeline:

1. **User Flow**:
   - WhatsApp query → Translate → Analyze → Retrieve Data → Generate Response → Translate → Reply

2. **Sensor Flow**:
   - Sensor → Gateway → Cloud → DB → Analyzer → Advice Trigger

---

## 📊 Dashboard

![Sensor Dashboard](images/dash-graph.png)

In addition to chatbot interactions, a live web-based dashboard displays real-time sensor trends.

---

## 📈 Research Contributions

- **IoT-to-Chatbot Integration**: Real-time sensor fusion in LLM powered chat responses
- **Cultural Localization**: Voice, script and advice personalized to region and crop
- **Rigorous Technical Evaluation**:
  - **LLM-as-a-Judge** for response quality
  - **RAGAS** for RAG evaluation
  - **COMETKiwi and Googles MetricX-24** for translation quality

---

## 🧪 Field Deployment Outcomes

- **Duration**: Two deployments × 45 days (Sugarcane + Spinach)
- **Sites**: 1 commercial farm, 1 university farm
- **Participants**: 5 farmers (managers + field workers)
- **Findings**:
  - Traditional dashboards had low engagement
  - WhatsApp chatbot increased trust, accessibility and decision-making
  - Farmers responded positively to chatbot alerts, warnings and advice

---
