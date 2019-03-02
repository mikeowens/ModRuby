  def processForm()

    @req.log(APLOG_NOTICE, 'file upload')

    @req.set_content_type('text/html')

    # Get the multipart boundary
    boundary = @req.boundary()

    # Create the parser
    parser = RSP::Rfc2388::Parser.new()

    # Parse the form. The first argument is a file-like object (supports read()
    # and readline()). The second argument is the multi-part form boundary that
    # divides the content parts.
    parser.parse(@req,  boundary) do |part|

      # Look for a file argument. If exists, then we have a file upload.
      if part.fileName != nil
        
        # Open the file

        # Normally, you wouldn't want to save the file using the posted filename
        # as this can be error prone (with IE and all). Typically we would use
        # the form name (part.name). But for testing it is okay we control the
        # file we are parsing.

        upload_file = open("#{part.fileName}", 'w')
        
        # Read all of the data out of the content part. This ultimately operates
        # on the underlying file object (@req). By default, the content is read
        # out in 4k blocks.x
        bytes = 0
        part.read() do |content|
          bytes += content.size
          upload_file.write(content);
        end
        
        # Close
        upload_file.close()
      else
        
        # Else this is just simply content part of some kind and we will just
        # store the data in the part's internal buffer.
        part.read()
      end
      
      # Print what we did
      if part.fileName != nil
        @req.puts "file %-20s: %s" % [part.fileName, part.contentType]
      else
        @req.puts "%-25s: %s" % [part.name, part.data]
      end
    end    
  end
