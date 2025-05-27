import pool from './db.js';

// Get the latest reading record for a specific user
export async function getLatestReadingByUserID(user_id) {
  const query = `
    SELECT * FROM readings WHERE user_id = $1 ORDER BY created_at DESC LIMIT 1
  `;

  try {
    const result = await pool.query(query, [user_id]);
    return result.rows[0]; // Return only the latest record
  } catch (error) {
    console.error('Error: Could Not Get Latest Reading by User ID.', error);
    return null;
  }
}

// Get all readings from the database, starting from a specific reading_id
export async function getReadingsFromID(user_id, from_reading_id = 0) {
  const query = `
    SELECT * 
    FROM readings 
    WHERE user_id = $1 AND id > $2
    ORDER BY created_at ASC
  `;

  try {
    const result = await pool.query(query, [user_id, from_reading_id]);
    return result.rows; // Return all matching records
  } catch (error) {
    console.error('Error: Could Not Get Readings from Reading ID.', error);
    return [];
  }
}