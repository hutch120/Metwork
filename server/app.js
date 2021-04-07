const express = require('express')
const app = express()
const port = 5000
const neatCsv = require('neat-csv')
const fs = require('fs').promises;
const { DateTime, Interval } = require("luxon");
const trim = require('lodash/trim')
const toNumber = require('lodash/toNumber')
const toLower = require('lodash/toLower')
const startsWith = require('lodash/startsWith')
const flatMap = require('lodash/flatMap')

let totalPacketsTWatchLastCall = 0

// loadStationsFromFile
async function loadData() {
    try {
        const csvString = await fs.readFile('./stations-01.csv', 'utf8')
        let csvStations = csvString.substring(csvString.indexOf('Station MAC'), csvString.length)
        csvStations = csvStations.split(', ').join(',')
        csvStations = csvStations.split('# ').join('')
        let stations = await neatCsv(csvStations)

        let totalPacketsTWatch = 0
        var dtNow = DateTime.now();
        stations = flatMap(stations, station => {
            let stationNew = {}
            stationNew['serverTimeUTC'] = dtNow.toUTC()
            // console.log('station', station)
            const keys = Object.keys(station)
            keys.map(key => {
                let value = toNumber(trim(station[key]))
                if (!value) value = trim(station[key])
                switch (toLower(key).split(' ').join('')) {
                    case 'bssid':
                        if (value === '(not associated)') {
                            stationNew['bssid'] = null
                            stationNew['bssidNotAssociated'] = true
                        } else {
                            stationNew['bssid'] = value
                        }
                        break
                    case 'firsttimeseen':
                        value += 'Z' // Makes the parser see it as UTC (which it is.. rather than try to convert it to some server offset)
                        // stationNew['first-time-seen-raw'] = value
                        const dtFTS = DateTime.fromSQL(value)
                        const iFTS = Interval.fromDateTimes(dtFTS, dtNow)
                        stationNew['firstTimeSeenUTC'] = dtFTS.toUTC()
                        stationNew['firstTimeSeenAgoSeconds'] = Math.round(iFTS.length('seconds'))
                        break
                    case 'lasttimeseen':
                        value += 'Z' // Makes the parser see it as UTC (which it is.. rather than try to convert it to some server offset)
                        // stationNew['last-time-seen-raw'] = value
                        const dtLTS = DateTime.fromSQL(value)
                        const iLTS = Interval.fromDateTimes(dtLTS, dtNow)
                        stationNew['lastTimeSeenUTC'] = dtLTS.toUTC()
                        stationNew['lastTimeSeenAgoSeconds'] = Math.round(iLTS.length('seconds'))
                        break
                    case 'power':
                        stationNew['power'] = value
                        break
                    case 'probedessids':
                        stationNew['probedEssids'] = value
                        break
                    case 'stationmac':
                        // https://maclookup.app/macaddress/3C6105
                        if (startsWith(value, '3C:61:05')) {
                            stationNew['MABL'] = 'espressif'
                            stationNew['OUI'] = 'espressif'
                            // Highly likely at least ... should try to examine the packet is ESPNow... but data not available through airodump-ng
                            stationNew['tWatch2020'] = true
                            totalPacketsTWatch += toNumber(trim(station['packets']))
                        } else {
                            stationNew['MABL'] = ''
                            stationNew['OUI'] = ''
                            // Highly likely at least ... should try to examine the packet is ESPNow... but data not available through airodump-ng
                            stationNew['tWatch2020'] = false
                        }
                        stationNew['stationMac'] = value
                        break
                    case 'packets':
                        stationNew['packets'] = value
                        break
                    default:
                }
            })
            if (stationNew['tWatch2020']) {
                return [stationNew]
            }
            return []
        })


        const returnData = { /*stations,*/ meta: { totalPacketsTWatch, totalPacketsTWatchLastCall, totalPacketsTWatchDiff: totalPacketsTWatch - totalPacketsTWatchLastCall } }
        totalPacketsTWatchLastCall = totalPacketsTWatch
        console.log('totalPacketsTWatch', totalPacketsTWatch)
        return returnData
    } catch (e) {
        console.log('error', e)
    }
    return null
}

app.use(express.static('client/build')) // Build the react site to serve from this app.

app.get('/stations', async (req, res) => {
    const data = await loadData()
    if (!data) {
        return res.json({ success: false })
    }
    res.json({ ...data, success: true })
})

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
})

