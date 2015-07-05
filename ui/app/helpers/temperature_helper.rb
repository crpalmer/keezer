module TemperatureHelper
  def keezer_temperature
    number_with_precision(File.read('/mnt/1wire/28.FF3DCC6B1403/fasttemp'), precision: 1)
    rescue
       '???'
  end

  def tower_temperature
    number_with_precision(File.read('/mnt/1wire/28.FFAA976B1403/fasttemp'), precision: 1)
    rescue
       '???'
  end

  def chamber_temperature
    '???'
  end

  def wort_temperature
    '???'
  end
end
