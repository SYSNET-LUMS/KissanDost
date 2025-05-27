// public/refresh.js
document.addEventListener("DOMContentLoaded", () => {
    const fetchLatestReading = async () => {
      try {
        const response = await fetch('/api/latest-reading/1'); // Replace `1` with dynamic user_id as needed
        const data = await response.json();
  
        if (response.ok && data) {
          document.getElementById("pH").textContent = data.pH || 'N/A';
          document.getElementById("nitrogen").textContent = data.nitrogen || 'N/A';
          document.getElementById("phosphorus").textContent = data.phosphorus || 'N/A';
          document.getElementById("potassium").textContent = data.potassium || 'N/A';
          document.getElementById("temperature").textContent = data.temperature || 'N/A';
          document.getElementById("moisture").textContent = data.moisture || 'N/A';
          document.getElementById("conductivity").textContent = data.conductivity || 'N/A';

          // Update the reading time
          const rawTime = data.created_at;
          const localTime = rawTime ? formatToLocalTime(rawTime) : 'N/A';
          document.getElementById("reading-time").textContent = `${localTime}`;

          console.log(`Data successfully updated at ${new Date().toLocaleTimeString()}`);
          console.log(data)
        }
      } catch (error) {
        console.error("Error fetching latest reading:", error);
      }
    };

    const formatToLocalTime = (isoString) => {
      const date = new Date(isoString);
    
      // Format the updated time
      return date.toLocaleTimeString('en-US', {
        hour: '2-digit',
        minute: '2-digit',
        hour12: true,
      });
    };

    const graphContainer = document.getElementById("graph-container");
    const graphCanvas = document.getElementById("reading-graph");
    let chartInstance;

    // Mapping of reading types to their Urdu names
    const urduLabels = {
      ph: "پی ایچ",
      nitrogen: "نائٹروجن",
      phosphorus: "فاسفورس",
      potassium: "پوٹاشیم",
      temperature: "درجہ حرارت",
      moisture: "نمی",
      conductivity: "برقی ترسیلیت",
    };

    // Function to fetch readings from the server
    const fetchReadings = async (user_id) => {
      try {
        const response = await fetch(`/api/readings/${user_id}`);
        if (response.ok) {
          return await response.json();
        } else {
          console.error("Failed to fetch readings");
          return [];
        }
      } catch (error) {
        console.error("Error fetching readings:", error);
        return [];
      }
    };

    const renderGraph = (data, type) => {
      // Filter out readings with zero values for the selected type
      const filteredData = data.filter((reading) => reading[type] !== 0);
    
      const labels = filteredData.map((reading) => {
        const date = new Date(reading.created_at);
        return date.toLocaleDateString("en-GB", {
          day: "2-digit",
          month: "2-digit",
        }); // Example: "11/11"
      });
    
      const values = filteredData.map((reading) =>
        parseFloat(reading[type].toFixed(2)) // Round values to 2 decimals
      );
    
      if (chartInstance) {
        chartInstance.destroy(); // Destroy the previous chart to avoid duplication
      }
    
      chartInstance = new Chart(graphCanvas, {
        type: "line",
        data: {
          labels,
          datasets: [
            {
              label: urduLabels[type] || type, // Use Urdu label if available
              data: values,
              borderColor: "rgba(75, 192, 192, 1)",
              backgroundColor: "rgba(75, 192, 192, 0.2)",
              borderWidth: 2,
              tension: 0.4, // Smooth the line
              pointRadius: 0, // Remove dots from the graph
            },
          ],
        },
        options: {
          responsive: true,
          scales: {
            x: {
              title: {
                display: true,
                text: "تاریخ", // "Date" in Urdu
                padding: {
                  top: 10, // Add space between the title and the labels
                },
              },
              ticks: {
                font: { size: 12 }, // Adjust font size for better readability
                maxRotation: 0, // Prevent label rotation
                autoSkip: true, // Skip overlapping labels
                padding: 10, // Add space between the x-axis labels and the axis
              },
            },
            y: {
              title: { display: true, text: urduLabels[type] || type },
              ticks: { font: { size: 12 } },
            },
          },
        },
      });
    };
    
    // Add event listeners to all cards
    const cards = document.querySelectorAll(".card");
    cards.forEach((card) => {
      card.addEventListener("click", async (event) => {
        event.stopPropagation(); // Prevent event from propagating to the body

        const type = card.getAttribute("data-type"); // Get the type of reading (e.g., 'ph', 'nitrogen')
        const readings = await fetchReadings(1); // Replace `2` with the dynamic user ID
        renderGraph(readings, type);

        // Show the graph container
        graphContainer.style.display = "block";
      });
    });

    // Hide the graph when clicking anywhere outside the cards or graph
    document.body.addEventListener("click", (event) => {
      const isCardClick = event.target.closest(".card"); // Check if the click is on a card
      const isGraphClick = graphContainer.contains(event.target); // Check if the click is on the graph container

      if (!isCardClick && !isGraphClick) {
        graphContainer.style.display = "none"; // Hide the graph container
      }
    });
  
    // Fetch data initially and then every 30 minutes (1800000 milliseconds)
    fetchLatestReading();
    setInterval(fetchLatestReading, 1800000);
  });