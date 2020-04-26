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
    number_with_precision(File.read('/mnt/1wire/28.FFCEBF6B1403/fasttemp'), precision: 1)
    rescue
       '???'
  end

  def wort_temperature
    number_with_precision(File.read('/mnt/1wire/28.FFB6AD6B1403/fasttemp'), precision: 1)
    rescue
       '???'
  end
end
