import React, { useEffect, useState } from 'react'
import Avatar from '@material-ui/core/Avatar';
import { deepPurple } from '@material-ui/core/colors';
import { makeStyles } from '@material-ui/core/styles'

// Note the proxy flag in package.json is how fetch works... TODO: test on the build version.
export default function App() {
  const classes = useStyles();
  const [data, setData] = useState()
  const [dataAvg, setDataAvg] = useState([0, 0, 0, 0, 0])

  useEffect(() => {
    async function getData() {
      try {
        const res = await window.fetch('/stations') // Calls on port 5000 due to proxy flag in package.json
        const json = await res.json()
        setData(json)
        let dataAvgClone = [...dataAvg]
        dataAvgClone.shift()
        dataAvgClone.push(json.meta.totalPacketsTWatchDiff)
        setDataAvg(dataAvgClone)
      } catch (err) {
        console.log('Failed to get stations', err)
      }
    }
    window.setInterval(getData, 1000)
  }, [])

  if (!data) {
    return 'Waiting for data...'
  }

  let avg = 0
  dataAvg.map(value => {
    avg += value
  })
  avg = avg / 5
  return (
    <div className={classes.root}>
      <Avatar className={classes.avatar}>{avg}</Avatar>
      <div className={classes.stats}>{data.meta.totalPacketsTWatch}</div>
    </div>
  );
}

const useStyles = makeStyles((theme) => ({
  root: {
    '& > *': {
      margin: theme.spacing(1),
    },
    background: 'black',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
    alignItems: 'center',
    textAlign: 'center',
    minHeight: '100vh'
  },
  avatar: {
    width: 200,
    height: 200,
    fontSize: 60,
    color: theme.palette.getContrastText(deepPurple[500]),
    backgroundColor: deepPurple[500],
  },
  stats: {
    color: 'white',
    fontSize: '12px',
    position: 'absolute',
    bottom: 16,
    right: 16
  }
}));