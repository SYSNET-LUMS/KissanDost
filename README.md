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

![System Architecture](KissanDost_Arch.png)

Kissan-Dost is composed of:

- **Data Collection**: Soil sensors (pH, NPK, moisture, EC, temperature)
- **Wireless Gateway**: ESP32 + LTE backhaul
- **Cloud Orchestration**: LLM agents for query analysis, translation, updates and response synthesis
- **Knowledge Sources**: Real-time weather, AMIS market prices, curated agronomic docs
- **LLM Interface**: Delivered via WhatsApp using OpenAI's Assistants API

---

## 🛠️ Hardware Deployment

![Sensor Module](sensor_module.png)

![Farm Deployments](Farms.png)

The IoT hardware stack includes:

- ESP32-powered sensor module with **7-in-1 RS485 suite**
- Field-deployable, waterproof enclosures (IP65)
- Deployed across **2 farms**: sugarcane and spinach plots, with real field feedback

---

## 💬 Conversational Interaction

![WhatsApp Chat](whatsapp_example1.png)

The system detects intent, retrieves relevant sensor and contextual data, and delivers **voice or text-based advice** accordingly.

---

## 🔁 User Interaction & Data Flow

![Flow Diagram](flow_diagram.png)

The system follows a dual-pipeline:

1. **User Flow**:
   - WhatsApp query → Translate → Analyze → Retrieve Data → Generate Response → Translate → Reply

2. **Sensor Flow**:
   - Sensor → Gateway → Cloud → DB → Analyzer → Advice Trigger

---

## 📊 Dashboard

![Sensor Dashboard](dash-graph.png)

In addition to chatbot interactions, a live web-based dashboard displays real-time sensor trends.

---

## 📈 Research Contributions

- **IoT-to-Chatbot Integration**: Real-time sensor fusion in chat responses
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
