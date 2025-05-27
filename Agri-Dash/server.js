// server.js
import express from 'express';
import path from 'path';
import { getLatestReadingByUserID, getReadingsFromID } from './db_controller.js';
import dotenv from 'dotenv';
import pool from './db.js';

dotenv.config();

const app = express();
const PORT = process.env.PORT || 5000;

// Set EJS as the view engine and serve static files
app.set('view engine', 'ejs');
app.use(express.json()); // This parses incoming JSON requests
app.use(express.static(path.join(path.resolve(), 'public')));

app.use((req, res, next) => {
  if (req.path === '/') {
    const sessionId = req.sessionID || "anonymous";
    const ipAddress = req.ip;
    const userAgent = req.headers['user-agent'];

    const query = `
      INSERT INTO app_usage (session_id, ip_address, user_agent, access_time)
      VALUES ($1, $2, $3, NOW())
    `;

    pool.query(query, [sessionId, ipAddress, userAgent])
      .then(() => console.log(`Access logged with session ID: ${sessionId}`))
      .catch((error) => console.error('Error logging access:', error));
  }
  next();
});

// Render the dashboard
app.get('/', async (req, res) => {
  try {
    const latestReading = await getLatestReadingByUserID(1); // Use any sample user_id for testing
    res.render('index', { reading: latestReading });
  } catch (error) {
    res.status(500).send('Error loading dashboard');
  }
});

// API endpoint to get the latest reading
app.get('/api/latest-reading/:user_id', async (req, res) => {
  const { user_id } = req.params;
  try {
    const latestReading = await getLatestReadingByUserID(user_id);
    if (latestReading) {
      res.json(latestReading);
    } else {
      res.status(404).json({ message: 'Reading not found' });
    }
  } catch (error) {
    res.status(500).json({ message: 'Server Error' });
  }
});

app.get('/api/readings/:user_id', async (req, res) => {
  const { user_id } = req.params;

  try {
    const readings = await getReadingsFromID(user_id, 10); // Fetch all readings for the user
    res.json(readings);
  } catch (error) {
    console.error('Error fetching all readings:', error);
    res.status(500).json({ message: 'Server error' });
  }
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});