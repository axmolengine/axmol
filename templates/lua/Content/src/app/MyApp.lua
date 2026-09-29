
local MyApp = class("MyApp", ax.load("mvc").AppBase)

function MyApp:onCreate()
    math.randomseed(os.time())
end

return MyApp
