# Metwork Server

This is designed to run on a Raspberry Pi or similar to provide an interface for the ESPNow messaging system.

## Project setup

See https://medium.com/@maison.moa/setting-up-an-express-backend-server-for-create-react-app-bc7620b20a61

Basically to setup dev run the server and react clients...

Start server (runs on port 5000) From this folder.
 nodemon app.js

Start client (runs on port 3000)
 cd client
 yarn start

Note client uses a proxy flag in `client/package.json` to connect via port 5000 for API requests. This is only needed for dev.
For the deployed version the client build folder will be served from the same server, so it doesn't need this proxy.