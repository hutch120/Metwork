const express = require('express')
const app = express()
const port = 5000
const neatCsv = require('neat-csv')
const fs = require('fs').promises;

// loadStationsFromFile
async function loadStationsFromFile() {
    try {
        const csvString = await fs.readFile('data.csv', 'utf8')
        const csvStations = csvString.substring(csvString.indexOf('Station MAC'), csvString.length)
        const stations = await neatCsv(csvStations)
        return stations
    } catch (e) {
        console.log('error', e)
    }
    return null
}

app.use(express.static('client/build')) // Build the react site to serve from this app.

// TODO: Test if this should be removed if express.static serves the React index.html file.
app.get('/', (req, res) => {
    res.send('This is an API ... maybe you were looking for the React site? Maybe check port number in browser?')
})

app.get('/stations', async (req, res) => {
    const stations = await loadStationsFromFile()
    if (!stations) {
        return res.json({ success: false })
    }
    res.json({ stations, success: true })
})

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
})

