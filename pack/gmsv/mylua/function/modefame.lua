--经验计算
function FreeModeFame( charindex, exp2, feedpoint )
	--print("原始经验:"..exp2, "最终声望:"..feedpoint);
	local NoFameMap = {40001,40002,40003,40004,40005,40006,40007,40008,40009,40010,40011};--不可获得声望的地图
	
	
	return feedpoint;
end

function reload()
	mydata();
end

function mydata()
	
end

function main()
	mydata();
	magic.addLUAListFunction("重读声望设置", "reload", "", 1, "[重读声望设置]");
end
