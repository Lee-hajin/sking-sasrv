function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end
--��̯�жϣ�����1�������̯������0��ɰ�̯
function FreeStreetVendorName( charaindex, streetname )
	if char.getInt(charaindex,"��ͼ��") == 2005 or char.getInt(charaindex,"��ͼ��") == 2006 then
		char.TalkToCli(charaindex, -1, "��̯ʧ��,�õ�ͼ��ֹ��̯��", "��ɫ");
		return 1;
	end
	local nostreetname = {"gm","GM","Gm","��","�ǣ�","����","�˷�","����","����","��","�Ų�"}
	for i = 1, table.getn(nostreetname) do
		str, len = string.gsub(streetname, nostreetname[i], "")
		if len > 0 then
			char.TalkToCli(charaindex, -1, "[������ʾ]�Ƿ���������̯ʧ�ܣ���̯��������һЩ��������лл������", "��ɫ")
			return 1
		end
	end
	return 0
end

function FreeStreetVendorPet( charaindex, petindex )
	if char.getInt(charaindex,"��ͼ��") == 2005 or char.getInt(charaindex,"��ͼ��") == 2006 then
		char.TalkToCli(charaindex, -1, "��̯ʧ��,�õ�ͼ��ֹ��̯��", "��ɫ");
		return 1;
	end
	return 0
end

function FreeStreetVendorItem( charaindex, itemindex )
	if char.getInt(charaindex,"��ͼ��") == 2005 or char.getInt(charaindex,"��ͼ��") == 2006 then
		char.TalkToCli(charaindex, -1, "��̯ʧ��,�õ�ͼ��ֹ��̯��", "��ɫ");
		return 1;
	end
	
	return 0
end

function data()

end

function main()
	data()
end

